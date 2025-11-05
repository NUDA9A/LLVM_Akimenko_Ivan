fill_init
    XOR x1 x1 x1
fi_loop_x
    XOR x2 x2 x2
fi_loop_y
    RAND x3
    ANDi x3 x3 1

    RAND x4
    ANDi x4 x4 1

    AND x6 x3 x4

    STORE_FIELD x10

    INC_NEi x5 x2 500
    BR_COND x5 fi_loop_y
fi_loop_y_continue
    INC_NEi x5 x1 500
    BR_COND x5 fi_loop_x
main_loop
    XOR x1 x1 x1
de_loop_x
    XOR x2 x2 x2
de_loop_y
    LOAD_FIELD x3 x10
    BR_COND x3 de_alive
de_dead
    PUT_PIXEL x1 x2 0x0
    BR de_next_y
de_alive
    PUT_PIXEL x1 x2 0x00FFFFFF
de_next_y
    INC_NEi x5 x2 500
    BR_COND x5 de_loop_y
de_next_x
    INC_NEi x5 x1 500
    BR_COND x5 de_loop_x
de_next_x_continue
    FLUSH
update_field
    XOR x1 x1 x1
uf_loop1_x
    XOR x2 x2 x2
uf_loop1_y
    MOV x8 x1
    MOV x9 x2

    XOR x3 x3 x3

    MOVi x12 -1
ca_loop_i
    MOVi x13 -1
ca_loop_j
    D_EQi_AND x12 x13 0 0
    BR_COND x7 ca_skip_n
ca_check1
    D_EQi_AND x12 x8 -1 0
    BR_COND x7 ca_skip_n
ca_check2
    D_EQi_AND x12 x8 1 499
    BR_COND x7 ca_skip_n
ca_check3
    D_EQi_AND x13 x9 -1 0
    BR_COND x7 ca_skip_n
ca_check4
    D_EQi_AND x13 x9 1 499
    BR_COND x7 ca_skip_n
ca_continue
    MOV x1 x8
    ADD x1 x1 x12

    MOV x2 x9
    ADD x2 x2 x13

    LOAD_FIELD x4 x10
    BR_COND x4 ca_inc
ca_continue2
    BR ca_skip_n
ca_inc
    ADDi x3 x3 1
ca_skip_n
    INC_NEi x5 x13 2
    BR_COND x5 ca_loop_j
ca_skip_check1
    INC_NEi x5 x12 2
    BR_COND x5 ca_loop_i
ca_skip_continue
    MOV x1 x8
    MOV x2 x9

    LOAD_FIELD x4 x10
    BR_COND x4 uf_pixel_alive
uf_pixel_dead
    EQi x5 x3 3
    BR_COND x5 uf_set1
uf_pixel_dead_continue
    BR uf_set0
uf_pixel_alive
    EQi x5 x3 2
    EQi x6 x3 3
    OR x7 x5 x6
    BR_COND x7 uf_set1
uf_pixel_alive_continue
    BR uf_set0
uf_set1
    XOR x6 x6 x6
    ADDi x6 x6 1
    STORE_FIELD x11
    BR uf_next1_y
uf_set0
    XOR x6 x6 x6
    STORE_FIELD x11
    BR uf_next1_y
uf_next1_y
    INC_NEi x5 x2 500
    BR_COND x5 uf_loop1_y
uf_next1_x
    INC_NEi x5 x1 500
    BR_COND x5 uf_loop1_x
uf_next1_x_continue
    XOR x1 x1 x1
uf_loop2_x
    XOR x2 x2 x2
uf_loop2_y
    LOAD_FIELD x6 x11
    STORE_FIELD x10

    INC_NEi x5 x2 500
    BR_COND x5 uf_loop2_y
uf_next2_x
    INC_NEi x5 x1 500
    BR_COND x5 uf_loop2_x
uf_next2_x_continue
    BR main_loop