#include <stdio.h>

int main(void) {
    int ncmd, battery, * cmds, i = 0, tx, ty, curticks = 0, x = 0, y = 0, hits = 0, cc;
    scanf("%d %d", &ncmd, &battery);
    cmds = malloc(sizeof(int) * ncmd);
    for (; i < ncmd; i++)
        scanf("%d", &cmds[i]);
    scanf("%d %d", &tx, &ty);
    for (; curticks < battery; curticks++) {
        cc = cmds[curticks % ncmd];
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
                y -= cc;
                break;
            case 3:
                if (ty == y && x - cc <= tx && tx < x)
                    hits++;
                x -= cc;
                break;
        }
    }
    printf("%d", hits);
}
