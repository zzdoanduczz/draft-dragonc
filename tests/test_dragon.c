#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "test_dragon.h"
#include "../src/dragon/dragon.h"
#include "../src/dragon/dragon_sbox.h"
#include "../src/dragon/dragon_f.h"
#include "../src/dragon/dragon_init.h"
#include "../src/dragon/dragon_cipher.h"

int test_sbox_and_G(const char *filepath) {
    printf("TEST G_box & H_box (TU FILE: %s)\n", filepath);

    // 1. Kiểm tra nhanh phần tử đầu và cuối của 2 bảng tra cứu tĩnh
    if (sbox1[0] != 0x393BCE6B || sbox1[255] != 0x0192B555 ||
        sbox2[0] != 0xA94BC384 || sbox2[255] != 0x51561E77) {
        printf("[FAIL] Du lieu bang sbox1 hoac sbox2 bi sai!\n");
        return 0;
    }

    // 2. Mở file chứa các test case
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("[ERROR] Khong the mo file test: %s\n", filepath);
        return 0;
    }

    char line[256];
    int line_num = 0;
    int total = 0;
    int passed_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        // Bỏ qua khoảng trắng đầu dòng
        char *p = line;
        while (isspace((unsigned char)*p)) p++;

        // Bỏ qua dòng trống hoặc dòng chú thích (#)
        if (*p == '\0' || *p == '#') {
            continue;
        }

        char type;
        int n;
        uint32_t input, expected;

        // Đọc 4 trường: FUNC N INPUT EXPECTED
        if (sscanf(p, " %c %d %x %x", &type, &n, &input, &expected) != 4) {
            printf("[WARN] Dong %d sai dinh dang, bo qua: %s", line_num, line);
            continue;
        }

        type = (char)toupper((unsigned char)type);
        if ((type != 'G' && type != 'H') || n < 1 || n > 3) {
            printf("[WARN] Dong %d tham so khong hop le (%c, n=%d), bo qua.\n", line_num, type, n);
            continue;
        }

        uint32_t actual = (type == 'G') ? G_box(input, n) : H_box(input, n);
        total++;

        if (actual == expected) {
            passed_count++;
            printf("[PASS] #%02d | %c_%d(0x%08X) = 0x%08X\n",
                   total, type, n, input, actual);
        } else {
            printf("[FAIL] #%02d | %c_%d(0x%08X): Thuc te = 0x%08X | Ky vong = 0x%08X\n",
                   total, type, n, input, actual, expected);
        }
    }

    fclose(fp);

    printf("Ket qua test: %d/%d test dat.\n\n", passed_count, total);
    return (total > 0 && passed_count == total) ? 1 : 0;
}
