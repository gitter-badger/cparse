#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <cparse/error.h>
#include <cparse/parse.h>
#include <cparse/object.h>
#include "parse.test.h"

extern const char *cparse_app_id;

extern const char *cparse_facebook_app_id;

extern bool cparse_offline_messages;

extern bool cparse_error_messages;

extern int cparse_client_request_timeout;

struct obj_list
{
    struct obj_list *next;
    cParseObject *obj;
};

struct obj_list *first_obj = NULL;

cParseObject *cparse_new_test_object(const char *name, int score)
{
    cParseObject *obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_string(obj, "playerName", name);
    cparse_object_set_number(obj, "score", score);

    return obj;
}

int cparse_cleanup_test_object(cParseObject *obj)
{
    struct obj_list *node = malloc(sizeof(struct obj_list));

    node->obj = obj;

    node->next = first_obj;
    first_obj = node;

    return 1;
}

int cparse_save_test_object(cParseObject *obj)
{
    cParseError *error = NULL;
    bool rval = false;

    cparse_client_request_timeout = 10;

    rval = cparse_object_save(obj, &error);

    if (!rval || error)
    {
        if (error)
        {
            printf("Save error: %s\n", cparse_error_message(error));

            cparse_error_free(error);
        }
        return 0;
    }

    return cparse_cleanup_test_object(obj);
}

int cparse_cleanup_test_objects()
{
    struct obj_list *node, *next_node;

    for (node = first_obj; node != NULL; node = next_node)
    {
        next_node = node->next;

        if (node->obj)
        {
            cParseError *error = NULL;

            if (!cparse_object_delete(node->obj, &error))
            {
                if (error) {
                    printf("delete error: %s\n", cparse_error_message(error));
                }
            }

            cparse_object_free(node->obj);
        }
        if (node == first_obj)
        {
            if (next_node) {
                first_obj = next_node;
            }
            else {
                first_obj = NULL;
            }
        }
        free(node);
    }

    return 1;
}

int cparse_create_and_save_test_object(const char *name, int score)
{
    cParseObject *obj = cparse_new_test_object(name, score);

    return cparse_save_test_object(obj);
}

const char* rand_name()
{
    static char buf[100];

    int fuzz = rand() % 255;

    switch (rand() % 4) {
    case 0:
        snprintf(buf, sizeof(buf), "frank%03d", fuzz);
        break;
    case 1:
        snprintf(buf, sizeof(buf), "bobby%03d", fuzz);
        break;
    case 2:
        snprintf(buf, sizeof(buf), "jenn%03d", fuzz);
        break;
    case 3:
        snprintf(buf, sizeof(buf), "angie%03d", fuzz);
        break;
    }
    return buf;
}


static void cparse_test_setup()
{

}

static void cparse_test_teardown()
{

}

Suite *cparse_parse_suite (void)
{
    Suite *s = suite_create ("Config");

    /* Core test case */
    TCase *tc = tcase_create ("Config");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    suite_add_tcase(s, tc);

    return s;
}

