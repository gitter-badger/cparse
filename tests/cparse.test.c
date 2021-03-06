
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <cparse/json.h>
#include <time.h>
#include <cparse/parse.h>

Suite *cparse_parse_suite();
Suite *cparse_json_suite();
Suite *cparse_object_suite();
Suite *cparse_query_suite();
Suite *cparse_util_suite();
Suite *cparse_user_suite ();
Suite *cparse_client_suite ();
Suite *cparse_acl_suite ();
Suite *cparse_role_suite ();

extern int cparse_cleanup_test_objects();
extern const char *cparse_app_id;
extern const char *cparse_api_key;

void read_env_config();
void read_test_config();
void die(const char *message);
void cleanup();

int main(void)
{
    int number_failed;
    SRunner *sr;

    srand(time(0));

    read_test_config();

    read_env_config();

    if (cparse_app_id == NULL) {
        die("application id not set");
    }

    if (cparse_api_key == NULL) {
        die("api key not set");
    }

#ifdef DEBUG
    cparse_set_log_level(cParseLogTrace);
#else
    cparse_set_log_level(cParseLogError);
#endif

    sr = srunner_create(cparse_parse_suite());
    srunner_add_suite(sr, cparse_json_suite());
    srunner_add_suite(sr, cparse_object_suite());
    srunner_add_suite(sr, cparse_query_suite());
    srunner_add_suite(sr, cparse_util_suite());
    srunner_add_suite(sr, cparse_user_suite());
    srunner_add_suite(sr, cparse_client_suite());
    srunner_add_suite(sr, cparse_acl_suite());
    srunner_add_suite(sr, cparse_role_suite());
    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    free( (char*) cparse_app_id);
    free( (char*) cparse_api_key);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


void die(const char *message)
{
    printf("ERROR: %s\n", message);
    abort();
}

void read_test_config()
{
    FILE *file;
    char *text;
    long fsize;
    cParseJson *config;

    file = fopen(ROOT_PATH "/tests/parse.test.json", "rb");

    if (!file)
    {
        file = fopen(ROOT_PATH "/parse.test.json", "rb");

        if (!file)
        {
            puts("parse.test.json not found");
            return;
        }
    }

    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    text = malloc(sizeof(char) * fsize + 1);

    memset(text, 0, sizeof(char) * fsize + 1);

    if (!fread(text, sizeof(char), fsize, file))
    {
        die("error reading file");
    }

    fclose(file);

    config = cparse_json_tokenize(text);

    free(text);

    if (cparse_json_contains(config, "parseAppId")) {
        cparse_set_application_id(cparse_json_get_string(config, "parseAppId"));
    }
    else {
        die("No app id");
    }

    if (cparse_json_contains(config, "parseApiKey")) {
        cparse_set_api_key(cparse_json_get_string(config, "parseApiKey"));
    }
    else {
        die("No api key");
    }

    cparse_json_free(config);
}

void read_env_config()
{
    const char *val = getenv("PARSE_APP_ID");

    if (val != NULL) {
        cparse_set_application_id(val);
    }

    val = getenv("PARSE_API_KEY");

    if (val != NULL) {
        cparse_set_api_key(val);
    }
}

void cleanup()
{
    if (cparse_app_id) {
        free((char *)cparse_app_id);
    }

    if (cparse_api_key) {
        free((char *)cparse_api_key);
    }
}
