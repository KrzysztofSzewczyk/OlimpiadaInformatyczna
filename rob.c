
#include <stdio.h>
#include <stdlib.h>

int rn(void) {
    register int n = 0, c = getchar();
    
    while (!('0' <= c && c <= '9')) c = getchar();
    while ('0' <= c && c <= '9') n = n * 10 + c - '0', c = getchar();
    return n;
}

int main(void) {
    int ib[12], p = 0;
    register int ncmd = rn(), battery = rn(), * cmds = malloc(sizeof(int) * ncmd), i = 0, tx, ty, curticks = 0, x = 0, y = 0, hits = 0, cc;
    for (; i < ncmd; i++)
        cmds[i] = rn();
    tx = rn(), ty = rn();
    for (; curticks < battery; curticks++) {
        cc = cmds[curticks % ncmd];
        if(curticks % 4 > 1)
            cc = -cc;
        switch (curticks % 4) {
            case 0:
                if (tx == x && y < ty && ty <= y + cc)
                    hits++;
                y += cc;
                break;
            case 1:
                if (ty == y && x < tx && tx <= x + cc)
                    hits++;
                x += cc;
                break;
            case 2:
                if (tx == x && y - cc <= ty && ty < y)
                    hits++;
                y += cc;
                break;
            case 3:
                if (ty == y && x - cc <= tx && tx < x)
                    hits++;
                x += cc;
                break;
        }
    }

    if(!hits)
        putchar(48);
    else {
        while(hits) ib[p++] = (hits % 10) + 48, hits /= 10;
        while(p--) putchar(ib[p]);
    }
}
