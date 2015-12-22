#include <stdio.h>
#include <check.h>
#include "check_jstruct.h"
#include "check_import.h"
#include <jstruct/jstruct.h>
#include <jstruct/jstruct_private.h>
#include <jstruct/import.h>
#include "data/basic.h"

#define BIG_INT64 0xFFFFFFFFF

START_TEST(import_basic_data) {
    struct my_json_data data = make_data();
    fprintf(stdout, "my_data %ull\n", data.id);
    fflush(stdout);
    struct json_object *obj = make_json_obj();
    fprintf(stdout, "JSON INPUT (basic): %s\n", json_object_to_json_string(obj));
    fflush(stdout);
    jstruct_import(obj, &data, my_json_data, NULL);
    fprintf(stdout, "my_data %ull\n", data.id);
    fflush(stdout);
    test_data(data, obj);
    json_object_put(obj);
} END_TEST

START_TEST(import_struct_data_with_errors) {
    // TODO: do this
} END_TEST

START_TEST(import_struct_data) {
// TODO: Milestone 5 is nested and array data (feature complete!)
#ifdef M5
    struct my_json_data data = make_data();
    struct my_json_container c = {0};
    int i;
    c.main_data = data;
    for (i = 0; i < ARRAYSIZE(c.array_data); ++i) {
        ++data.id;
        c.array_data[i] = data;
    }
    /* malloc macro (automatically sets container.array_data__length__ = 2) */
    jstruct_array_malloc(c, alloc_array_data, struct my_json_data, 2);
    for (i = 0; i < c.alloc_array_data__length__; ++i) {
        ++data.id;
        c.alloc_array_data[i] = data;
    }

    struct json_object *obj = jstruct_import(&c, my_json_container);
    struct json_object *prop;
    struct json_object *item;

    fprintf(stdout, "JSON OUTPUT: %s\n", json_object_to_json_string(obj));
    fflush(stdout);
    ck_assert(json_object_object_get_ex(obj, "main_data", &prop) == true);
    test_data(c.main_data, prop);

    ck_assert(json_object_object_get_ex(obj, "array_data", &prop) == true);
    ck_assert(json_object_array_length(prop) == ARRAYSIZE(c.array_data));
    for (i = 0; i < ARRAYSIZE(c.array_data); ++i) {
        item = json_object_array_get_idx(prop, i);
        ck_assert_ptr_ne(item, NULL);
        test_data(c.array_data[i], item);
    }
    ck_assert(json_object_object_get_ex(obj, "alloc_array_data", &prop) == true);
    ck_assert(json_object_array_length(prop) == c.alloc_array_data__length__);
    for (i = 0; i < c.alloc_array_data__length__; ++i) {
        item = json_object_array_get_idx(prop, i);
        test_data(c.alloc_array_data[i], item);
    }
    // TODO: autofree
    free(c.alloc_array_data);
    json_object_put(obj);
#endif

} END_TEST

TCase *import_test_case(void) {
    TCase *tc = tcase_create("import");

    tcase_add_test(tc, import_basic_data);
    tcase_add_test(tc, import_struct_data_with_errors);
    tcase_add_test(tc, import_struct_data);

    return tc;
};
