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

#ifndef __ASN1_PER_H
#define __ASN1_PER_H

struct asn1_per_buffer {
	const unsigned char *data;
	unsigned length, i, bit;
	int error;
};

struct asn1_per_bitmap {
	unsigned char data[16];
};

struct asn1_per_sequence_header {
	int extended;
	struct asn1_per_bitmap present;
};

struct asn1_per_sequence_extension_header {
	unsigned count;
	struct asn1_per_bitmap present;
};

void asn1_per_initialize(struct asn1_per_buffer *bb,
			 const unsigned char *data,
			 unsigned length, unsigned position);

int asn1_per_read_bit(struct asn1_per_buffer *bb);

unsigned asn1_per_read_bits(struct asn1_per_buffer *bb, unsigned count);

void asn1_per_read_bitmap(struct asn1_per_buffer *bb, unsigned count,
			  struct asn1_per_bitmap *bitmap);

void asn1_per_read_bytes(struct asn1_per_buffer *bb,
			 void *buffer, unsigned count);

void asn1_per_byte_align(struct asn1_per_buffer *bb);

unsigned asn1_per_read_unsigned(struct asn1_per_buffer *bb,
				unsigned lower, unsigned upper);

unsigned asn1_per_read_length(struct asn1_per_buffer *bb,
			      unsigned lower, unsigned upper);

unsigned asn1_per_read_small(struct asn1_per_buffer *bb);

unsigned asn1_per_read_choice_header(struct asn1_per_buffer *bb,
				     int extendable,
				     unsigned options, unsigned *after);

void asn1_per_read_sequence_header(struct asn1_per_buffer *bb, int extendable,
				   unsigned optional_count,
				   struct asn1_per_sequence_header *hdr);

void asn1_per_read_sequence_extension_header(struct asn1_per_buffer *bb,
					     const struct asn1_per_sequence_header *hdr,
					     struct asn1_per_sequence_extension_header *ext);

void asn1_per_skip_sequence_extension(struct asn1_per_buffer *bb,
				      const struct asn1_per_sequence_header *hdr);

void asn1_per_skip_object_id(struct asn1_per_buffer *bb);

unsigned asn1_per_read_octet_string_header(struct asn1_per_buffer *bb);

void asn1_per_skip_octet_string(struct asn1_per_buffer *bb);


int asn1_per_bitmap_get(const struct asn1_per_bitmap *bitmap, unsigned i);

#endif
