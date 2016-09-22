unsigned int TestMemory(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 386인가,  486이후인가의 확인 */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 386에서는 AC=1으로 해도 자동으로 0에 돌아와 버린다 */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* 캐쉬 금지 */
		store_cr0(cr0);
	}

	i = SubTestMemory(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* 캐쉬 허가 */
		store_cr0(cr0);
	}

	return i;
}

unsigned int SubTestMemory(unsigned int start, unsigned int end)
{
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for (i = start; i <= end; i += 0x1000) {
		p = (unsigned int *) (i + 0xffc);
		old = *p;			/* 이전의 값을 기억해 둔다 */
		*p = pat0;			/* 시험삼아 써 본다 */
		*p ^= 0xffffffff;	/* 그리고 그것을 반전해 본다 */
		if (*p != pat1) {	/* 반전 결과가 되었는지?  */
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;	/* 한번 더 반전해 본다 */
		if (*p != pat0) {	/* 원래대로 돌아갔는지?  */
			goto not_memory;
		}
		*p = old;			/* 원래대로 되돌린다 */
	}
	return i;
}
