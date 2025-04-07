int target_func(char *buf, int size) {
    printf("buffer:%p, size:%p\n", buf, size);
    switch (buf[0]) {
        case 1:
            puts("222");
            if (buf[1] == '0') {
                puts("null ptr deference");
                *(char *)(0) = 1;  // 空指针解引用
            }
            break;
        case 0:
            if (buf[2] == '0') {
                if (buf[1] == '0') {
                    puts("crash....");
                    *(char *)(0xdeadbeef) = 1;  // 无效内存访问
                }
            }
            break;
        default:
            puts("default action");
            break;
    }
    return 1;
}
