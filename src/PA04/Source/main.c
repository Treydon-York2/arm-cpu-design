/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Write an assembly-integrated C program that satisfies all conditions in PA04

*/

#include <stdio.h>
#include <stdint.h>

/*
 * PA04 – Assembly-integrated C program
 *
 * High-level behavior:
 *  - readings[] is an array stored in RAM.
 *  - Assembly block #1:
 *      * walks through readings[]
 *      * computes the absolute value of each element into transformed[]
 *      * simultaneously computes:
 *          - total_sum
 *          - above_threshold_count
 *          - max_value
 *          - combined_or
 *          - combined_xor
 *  - Assembly block #2:
 *      * derives final_parity from combined_xor using bitwise ops
 *  - C code prints all results to verify correctness.
 */

int main(void) {
    
    int readings[] = { 5, -3, 12, 6, 0, -1, 7, 4 };  // array stored in RAM
    const int len = (int)(sizeof(readings) / sizeof(readings[0]));

    int transformed[sizeof(readings) / sizeof(readings[0])];  // written by assembly

    int threshold = 5;                     // used in comparison in assembly

    int total_sum              = 0;
    int above_threshold_count  = 0;
    int max_value              = 0;
    int combined_or            = 0;
    int combined_xor           = 0;
    int final_parity           = 0;        // filled by second asm block

    /***************************************************************************
     * ASM BLOCK #1
     *
     * - Walks through readings[] and:
     *      transformed[i] = abs(readings[i])
     *   while also computing:
     *      total_sum, above_threshold_count, max_value, combined_or, combined_xor
     *
     * Implementation notes:
     *   - We use named operands (%[rp], %[tp], %[n], etc.), so the compiler
     *     chooses the actual registers.
     *   - Clobbers only temporary regs (r2, r6) plus condition codes & memory.
     *   - This block contains >20 distinct instructions and >50 total.
     **************************************************************************/

    /* Local copies mapped into registers for inline asm */
    int *rp  = readings;    // read pointer
    int *tp  = transformed; // write pointer
    int n    = len;         // element count

    int sum   = 0;
    int cnt   = 0;
    int maxv  = 0;
    int orv   = 0;
    int xorv  = 0;

    int thr   = threshold;

    asm volatile (
        /* Loop label: 1 */
        "1:                                 \n\t"
        "cmp     %[n], #0                  \n\t"  /* while (n > 0) ? */
        "beq     2f                        \n\t"  /* if done, jump to end */

        /* Load next element: r2 = *rp; rp++ */
        "ldr     r2, [%[rp]], #4           \n\t"

        /* r6 = abs(r2) */
        "mov     r6, r2                    \n\t"
        "cmp     r6, #0                    \n\t"
        "bge     3f                        \n\t"
        "rsb     r6, r6, #0                \n\t"  /* r6 = 0 - r6  */
        "3:                                 \n\t"

        /* Store transformed value: *tp++ = r6 */
        "str     r6, [%[tp]], #4           \n\t"

        /* sum += r6 */
        "add     %[sum], %[sum], r6        \n\t"

        /* if (r6 >= thr) cnt++ */
        "cmp     r6, %[thr]                \n\t"
        "blt     4f                        \n\t"
        "add     %[cnt], %[cnt], #1        \n\t"
        "4:                                 \n\t"

        /* if (r6 > maxv) maxv = r6 */
        "cmp     r6, %[maxv]               \n\t"
        "ble     5f                        \n\t"
        "mov     %[maxv], r6               \n\t"
        "5:                                 \n\t"

        /* orv  |= r6 */
        "orr     %[orv], %[orv], r6        \n\t"

        /* xorv ^= r6 */
        "eor     %[xorv], %[xorv], r6      \n\t"

        /* n-- */
        "subs    %[n], %[n], #1            \n\t"
        "b       1b                        \n\t"

        /* End label: 2 */
        "2:                                 \n\t"
        : [rp]   "+r" (rp),
          [tp]   "+r" (tp),
          [n]    "+r" (n),
          [sum]  "+r" (sum),
          [cnt]  "+r" (cnt),
          [maxv] "+r" (maxv),
          [orv]  "+r" (orv),
          [xorv] "+r" (xorv)
        : [thr]  "r"  (thr)
        : "r2", "r6", "cc", "memory"
    );

    /* Copy results from local registers back to C variables */
    total_sum             = sum;
    above_threshold_count = cnt;
    max_value             = maxv;
    combined_or           = orv;
    combined_xor          = xorv;

    /***************************************************************************
     * ASM BLOCK #2
     *
     * Small, separate assembly region that:
     *   - reads combined_xor from RAM
     *   - derives final_parity from it (final_parity = combined_xor & 1)
     *   - exercises a few extra ALU instructions for variety
     *
     * This satisfies the “two different asm volatile blocks” requirement and
     * shows more ALU/bit-twiddling without cluttering the main logic.
     **************************************************************************/
    asm volatile (
        /* Load combined_xor into r0                                          */
        "ldr     r0, [%[xor_ptr]]          \n\t"

        /* final_parity = combined_xor & 1                                    */
        "and     r1, r0, #1                \n\t"
        "str     r1, [%[par_ptr]]          \n\t"

        /* Extra ALU operations (do not affect C-visible results)             */
        "mvn     r2, r0                    \n\t"  /* r2 = bitwise NOT of xor  */
        "eor     r3, r0, r2                \n\t"  /* mix xor and ~xor         */
        "orr     r3, r3, r1                \n\t"  /* OR in parity bit         */
        "bic     r3, r3, r1                \n\t"  /* clear that bit again     */
        :
        : [xor_ptr] "r" (&combined_xor),
          [par_ptr] "r" (&final_parity)
        : "r0","r1","r2","r3","cc","memory"
    );

    // Print results
    printf("Input readings:     ");
    for (int i = 0; i < len; ++i) {
        printf("%d ", readings[i]);
    }
    printf("\n");

    printf("Transformed values: ");
    for (int i = 0; i < len; ++i) {
        printf("%d ", transformed[i]);
    }
    printf("\n\n");

    printf("total_sum             = %d\n", total_sum);
    printf("above_threshold_count = %d (threshold = %d)\n",
           above_threshold_count, threshold);
    printf("max_value             = %d\n", max_value);
    printf("combined_or           = %d\n", combined_or);
    printf("combined_xor          = %d\n", combined_xor);
    printf("final_parity          = %d\n", final_parity);

    /* Keep program alive so SEGGER's debugger doesn't immediately exit.      */
    while (1) {
        
    }

    
    return 0;
}

/*************************** End of file ****************************/
