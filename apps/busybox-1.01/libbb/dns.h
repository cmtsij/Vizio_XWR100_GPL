/*
  **
  ** dns.h - data structures used in dns queries and answers.
  **
  ** Part of the drpoxy package by Matthew Pratt. 
  **
  ** Copyright 1999 Matthew Pratt <mattpratt@yahoo.com>
  **
  ** This software is licensed under the terms of the GNU General 
  ** Public License (GPL). Please see the file COPYING for details.
  ** 
  **
*/

#ifndef DNS_H
#define DNS_H

struct dns_header{
  short int dns_id;
  short int dns_flags;
  short int dns_no_questions;
  short int dns_no_answers;
  short int dns_no_authority;
  short int dns_no_additional;
};

struct dns_answer{
  short int dns_name;
  short int dns_type;
  short int dns_class;
  short int dns_time_to_live;
  short int dns_time_to_live2;
  short int dns_data_len;
};

#endif
/* EOF */
