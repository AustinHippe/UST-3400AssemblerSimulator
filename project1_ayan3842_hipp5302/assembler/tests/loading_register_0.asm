                lw      0, 0, 1            # r1 <- valueA
                lw      2, 0, 1            # r2 <- valueB
                beq     1, 2, done      # if (valueA == valueB) skip to done
done            halt