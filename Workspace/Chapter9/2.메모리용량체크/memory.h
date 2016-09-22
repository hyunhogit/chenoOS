unsigned int TestMemory(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 386�ΰ�,  486�����ΰ��� Ȯ�� */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 386������ AC=1���� �ص� �ڵ����� 0�� ���ƿ� ������ */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* ĳ�� ���� */
		store_cr0(cr0);
	}

	i = SubTestMemory(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* ĳ�� �㰡 */
		store_cr0(cr0);
	}

	return i;
}

unsigned int SubTestMemory(unsigned int start, unsigned int end)
{
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for (i = start; i <= end; i += 0x1000) {
		p = (unsigned int *) (i + 0xffc);
		old = *p;			/* ������ ���� ����� �д� */
		*p = pat0;			/* ������ �� ���� */
		*p ^= 0xffffffff;	/* �׸��� �װ��� ������ ���� */
		if (*p != pat1) {	/* ���� ����� �Ǿ�����?  */
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;	/* �ѹ� �� ������ ���� */
		if (*p != pat0) {	/* ������� ���ư�����?  */
			goto not_memory;
		}
		*p = old;			/* ������� �ǵ����� */
	}
	return i;
}
