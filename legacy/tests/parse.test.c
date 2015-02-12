#include <check.h>
#include <stdio.h>
#include <cparse/error.h>
#include <cparse/parse.h>
#include <cparse/object.h>

#include "parse.test.h"

extern const char *cparse_app_id;

extern const char *cparse_facebook_app_id;

extern bool cparse_offline_messages;

extern bool cparse_error_messages;

struct obj_list
{
    struct obj_list *next;
    CPARSE_OBJ *obj;
};

struct obj_list *first_obj = NULL;

CPARSE_OBJ *cparse_new_test_object(const char *name, int score)
{
    CPARSE_OBJ *obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_string(obj, "playerName", name);
    cparse_object_set_number(obj, "score", score);

    return obj;
}

int cparse_cleanup_test_object(CPARSE_OBJ *obj)
{
    struct obj_list *node = malloc(sizeof(struct obj_list));

    node->obj = obj;

    node->next = first_obj;
    first_obj = node;

    int count = 0;

    for (struct obj_list *node = first_obj; node; node = node->next)
        count++;

    printf("Adding object %d for cleanup...\n", count);

    return 1;
}

int cparse_save_test_object(CPARSE_OBJ *obj)
{
    CPARSE_ERROR *error = NULL;

    int rval = cparse_object_save(obj, &error);

    if (!rval || error)
    {
        if (error)
        {
            if (error->message)
                printf("%s\n", error->message);

            cparse_error_free(error);
        }
        return 0;
    }

    return cparse_cleanup_test_object(obj);
}

int cparse_cleanup_test_objects()
{
    struct obj_list *node, *next_node;

    int count = 1;

    for (node = first_obj; node != NULL; node = next_node)
    {
        next_node = node->next;

        printf("cleaning up object %d...\n", count++);

        if (node->obj)
        {
            CPARSE_ERROR *error = NULL;

            if (!cparse_object_delete(node->obj, &error))
            {
                if (error)
                    printf("unable to delete object %s...\n", error->message);
            }

            cparse_object_free(node->obj);
        }
        if (node == first_obj)
        {
            if (next_node)
                first_obj = next_node;
            else
                first_obj = NULL;
        }
        free(node);
    }

    return 1;
}

int cparse_create_and_save_test_object(const char *name, int score)
{
    CPARSE_OBJ *obj = cparse_new_test_object(name, score);

    return cparse_save_test_object(obj);
}

static void cparse_test_setup()
{

}

static void cparse_test_teardown()
{

}

START_TEST(test_cparse_set_application_id)
{
    cparse_set_application_id("testId");

    fail_unless(!strcmp(cparse_app_id, "testId") );

}
END_TEST

START_TEST(test_cparse_set_facebook_application_id)
{
    cparse_set_facebook_application_id("testFacebookId");

    fail_unless(!strcmp(cparse_facebook_app_id, "testFacebookId"));
}
END_TEST

START_TEST(test_cparse_offline_messages_enabled)
{
    cparse_offline_messages_enabled(true);

    fail_unless(cparse_offline_messages == true);
}
END_TEST

START_TEST(test_cparse_error_messages_enabled)
{
    cparse_error_messages_enabled(true);

    fail_unless(cparse_error_messages == true);
}
END_TEST

Suite *cparse_parse_suite (void)
{
    Suite *s = suite_create ("Config");

    /* Core test case */
    TCase *tc = tcase_create ("Config");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_set_application_id);
    tcase_add_test(tc, test_cparse_set_facebook_application_id);
    tcase_add_test(tc, test_cparse_offline_messages_enabled);
    tcase_add_test(tc, test_cparse_error_messages_enabled);
    suite_add_tcase(s, tc);

    return s;
}