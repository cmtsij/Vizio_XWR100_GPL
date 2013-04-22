/*
 * Tiny ASN.1 packet encoding rules (PER) library.
 *
 * This is a tiny library which helps parsing ASN.1/PER packets
 * (i.e. read only). It is meant to be secure and small.
 *
 * Warning, this library may still be incomplete and buggy.
 *
 * (c) 2005 Max Kellermann <max@duempel.org>
 */

#include <linux/kernel.h>
#include <linux/string.h>

#include "asn1_per.h"

void asn1_per_initialize(struct asn1_per_buffer *bb,
			 const unsigned char *data,
			 unsigned length, unsigned position) {
	*bb = (struct asn1_per_buffer){
		.data = data,
		.length = length,
		.i = position,
		.bit = 8,
		.error = 0,
	};
}

int asn1_per_read_bit(struct asn1_per_buffer *bb) {
	int value;

	if (bb->error)
		return 0;

	if (bb->i >= bb->length) {
		bb->error = 1;
		return 0;
	}

	bb->bit--;

	value = (bb->data[bb->i] & (1 << bb->bit)) != 0;

	if (bb->bit == 0) {
		bb->bit = 8;
		bb->i++;
	}

	return value;
}

unsigned asn1_per_read_bits(struct asn1_per_buffer *bb, unsigned count) {
	unsigned value;

	if (bb->error)
		return 0;
	if (bb->i >= bb->length) {
		bb->error = 1;
		return 0;
	}

	if (count > 32) {
		/* XXX support more than 32 bits in the future here? */
		bb->error = 1;
		return 0;
	}

	if (count <= bb->bit) {
		value = (bb->data[bb->i] >> (bb->bit - count)) & (0xff >> (8 - count));

		bb->bit -= count;
		if (bb->bit == 0) {
			bb->bit = 8;
			bb->i++;
		}

		return value;
	}

	count -= bb->bit;

	value = bb->data[bb->i] & (0xff >> (8 - bb->bit));
	bb->i++;

	while (count >= 8) {
		if (bb->i >= bb->length) {
			bb->error = 1;
			return 0;
		}

		value = (value << 8) | bb->data[bb->i];

		bb->i++;
		count -= 8;
	}

	if (count > 0) {
		if (bb->i >= bb->length) {
			bb->error = 1;
			return 0;
		}

		value = (value << count) | (bb->data[bb->i] >> (8 - count));
	}

	bb->bit = 8 - count;

	return value;
}

void asn1_per_read_bitmap(struct asn1_per_buffer *bb, unsigned count,
			  struct asn1_per_bitmap *bitmap) {
	unsigned char *p;

	memset(bitmap, 0, sizeof(*bitmap));

	if (bb->error)
		return;

	if (count > sizeof(bitmap->data) * 8) {
		/* XXX limited bit map support */
		bb->error = 1;
		return;
	}

	for (p = bitmap->data; count > 8; count -= 8)
		*p++ = (unsigned char)asn1_per_read_bits(bb, 8);

	if (count > 0)
		*p = asn1_per_read_bits(bb, count) << (8 - count);

	return;
}

void asn1_per_read_bytes(struct asn1_per_buffer *bb,
			 void *buffer, unsigned count) {
	if (bb->error)
		return;

	if (bb->bit != 8) {
		bb->error = 1;
		return;
	}

	if (bb->i + count > bb->length) {
		bb->error = 1;
		return;
	}

	memcpy(buffer, bb->data + bb->i, count);

	bb->i += count;
}

void asn1_per_byte_align(struct asn1_per_buffer *bb) {
	if (bb->bit < 8) {
		bb->bit = 8;
		bb->i++;
	}
}

static unsigned count_bits(unsigned range) {
	unsigned bits = 0;

	if (range == 0)
		return 32;

	if (range == 1)
		return 1;

	while (bits < 32 && range > (unsigned)(1 << bits))
		bits++;

	return bits;
}

unsigned asn1_per_read_unsigned(struct asn1_per_buffer *bb,
				unsigned lower, unsigned upper) {
	unsigned range = (upper - lower) + 1;
	unsigned bits = count_bits(range);

	if (lower == upper)
		return lower;

	if (range == 0 || range > 255) {
		if (bits > 16)
			bits = asn1_per_read_length(bb, 1, (bits+7)/8) * 8;
		else if (bits > 8)
			bits = 16;
		asn1_per_byte_align(bb);
	}

	return lower + asn1_per_read_bits(bb, bits);
}

unsigned asn1_per_read_length(struct asn1_per_buffer *bb,
			      unsigned lower, unsigned upper) {
	if (upper < 65536)
		return asn1_per_read_unsigned(bb, lower, upper);

	asn1_per_byte_align(bb);

	if (!asn1_per_read_bit(bb))
		return asn1_per_read_bits(bb, 7);

	if (!asn1_per_read_bit(bb))
		return asn1_per_read_bits(bb, 14);

	bb->error = 1;
	return 0;
}

unsigned asn1_per_read_small(struct asn1_per_buffer *bb) {
	unsigned length;

	if (!asn1_per_read_bit(bb))
		return asn1_per_read_bits(bb, 6);

	length = asn1_per_read_length(bb, 0, INT_MAX);

	asn1_per_byte_align(bb);

	return asn1_per_read_bits(bb, length * 8);
}

unsigned asn1_per_read_choice_header(struct asn1_per_buffer *bb,
				     int extendable,
				     unsigned options, unsigned *after) {
	int extended;
	unsigned choice;

	extended = extendable && asn1_per_read_bit(bb);
	if (extended) {
		unsigned length;

		choice = asn1_per_read_small(bb) + options;
		length = asn1_per_read_length(bb, 0, INT_MAX);
		*after = bb->i + length;
	} else if (options < 2) {
		choice = 0;
		*after = 0;
	} else {
		choice = asn1_per_read_bits(bb, count_bits(options));
		*after = 0;
	}

	return choice;
}

void asn1_per_read_sequence_header(struct asn1_per_buffer *bb, int extendable,
				   unsigned optional_count,
				   struct asn1_per_sequence_header *hdr) {
	hdr->extended = extendable && asn1_per_read_bit(bb);
	asn1_per_read_bitmap(bb, optional_count, &hdr->present);
}

void asn1_per_read_sequence_extension_header(struct asn1_per_buffer *bb,
					     const struct asn1_per_sequence_header *hdr,
					     struct asn1_per_sequence_extension_header *ext) {
	if (!hdr->extended) {
		memset(ext, 0, sizeof(*ext));
		return;
	}

	ext->count = asn1_per_read_small(bb) + 1;
	if (bb->error)
		return;

	asn1_per_read_bitmap(bb, ext->count, &ext->present);
}

void asn1_per_skip_sequence_extension(struct asn1_per_buffer *bb,
				      const struct asn1_per_sequence_header *hdr) {
	struct asn1_per_sequence_extension_header ext;
	unsigned i;

	asn1_per_read_sequence_extension_header(bb, hdr, &ext);
	if (bb->error)
		return;

	for (i = 0; i < ext.count && !bb->error; i++) {
		if (asn1_per_bitmap_get(&ext.present, i))
			asn1_per_skip_octet_string(bb);
	}
}

void asn1_per_skip_object_id(struct asn1_per_buffer *bb) {
	unsigned length;

	length = asn1_per_read_length(bb, 0, 255);
	switch (length) {
	case 0:
		break;

	case 1:
		asn1_per_read_bits(bb, 8);
		break;

	case 2:
		asn1_per_read_bits(bb, 16);
		break;

	default:
		asn1_per_byte_align(bb);

		bb->i += length;
		if (bb->i > bb->length)
			bb->error = 1;
	}
}

unsigned asn1_per_read_octet_string_header(struct asn1_per_buffer *bb) {
	unsigned length;

	length = asn1_per_read_length(bb, 0, INT_MAX);
	if (length > 2)
		asn1_per_byte_align(bb);

	return length;
}

void asn1_per_skip_octet_string(struct asn1_per_buffer *bb) {
	unsigned length;

	length = asn1_per_read_length(bb, 0, INT_MAX);
	switch (length) {
	case 0:
		break;

	case 1:
		asn1_per_read_bits(bb, 8);
		break;

	case 2:
		asn1_per_read_bits(bb, 16);
		break;

	default:
		asn1_per_byte_align(bb);

		bb->i += length;
		if (bb->i > bb->length)
			bb->error = 1;
	}
}


int asn1_per_bitmap_get(const struct asn1_per_bitmap *bitmap, unsigned i) {
	if (i >= sizeof(bitmap->data) * 8)
		return 0;

	return (bitmap->data[i / 8] & (1 << (7 - (i % 8)))) != 0;
}
