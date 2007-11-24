#ifndef MAIL_H
#define MAIL_H
#include "data.h"

#define MAIL_GAVE_UP 99999
int mail_send(bt_project*, bt_message*, bt_element*, bt_element_type*);
int get_test_int(bt_project*, bt_message*, bt_element*, bt_element_type*);

#endif
