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

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* ĳ�� �㰡 */
		store_cr0(cr0);
	}

	return i;
}

void Init_MemoryManager(MemoryManager *man)
{
	man->frees = 0;		/* �� ���� ������ ���� */
	man->maxfrees = 0;		/* ��Ȳ �����룺frees�� �ִ�ġ */
	man->lostsize = 0;		/* �ع濡 ������ �հ� ������ */
	man->losts = 0;		/* �ع濡 ������ ȸ�� */
	return;
}

unsigned int Check_Size(MemoryManager *man)
/* �� ���� �������� �հ踦 ���� */
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int Alloc_Memory(MemoryManager *man, unsigned int size)
/* Ȯ�� */
{
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* ����� ������ �� ������ �߰� */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* free[i]�� ���������Ƿ� ������ ä��� */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* ����ü�� ���� */
				}
			}
			return a;
		}
	}
	return 0; /* �� ���� ���� */
}

int Free_Memory(MemoryManager *man, unsigned int addr, unsigned int size)
/* �ع� */
{
	int i, j;
	/* �����ϱ� ���� �����ϸ� free[]�� addr������ ������ �ִ� ���� ���� */
	/* �׷��ϱ� �켱 ��� �� ���ΰ��� �����Ѵ� */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {	/*������ ���� �ּҸ� ������ ������� �ִ�*/
		/* ���� ������� ������ ��� */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* ���� �� ������ ��ģ�� */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* ���� ������� ������ ��� */
				if (addr + size == man->free[i].addr) {
					/* �ڿ͵�  ��ģ�� */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]�� ���� */
					/* free[i]�� ���������Ƿ� �ϳ��� ������ ä���. */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* ����ü�� ���� */
					}
				}
			}
			return 0; /* ���� ���� */
		}
	}
	/* i=0�ΰ��, free[0]=addr�� �Ǿ�� �� . ���� �� ó�� ������� �Ǵ� ��. ������ ���� �ּҸ� ������ ����� ����*/
	if (i < man->frees) {
		/* ���� ������� ������ ��� */
		if (addr + size == man->free[i].addr) {
			/* �ڿ� ��ģ��. */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* ���� ���� */
		}
	}
	/* ������ ������� ����, ����� ������ �÷��� �ϴµ�, �ø� �� �ִ� ���*/
	if (man->frees < MEMMAN_FREES) {
		/* free[i]���� �ڸ�, �ڷ� �̵���Ű�� ���̸� ����� */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* �ִ�ġ�� ���� */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* ���� ���� */
	}
	/* �ø� ������ ���� ��� */
	man->losts++;
	man->lostsize += size;
	return -1; /* ���� ���� */
}


unsigned int Alloc_MemoryBlock(MemoryManager *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = Alloc_Memory(man, size);
	return a;
}

int Free_MemoryBlock(MemoryManager *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = Free_Memory(man, addr, size);
	return i;
}
