int main()
{
	int s = 0;
	int l = 5;
	int kil;
	s = s + l;
	l = s*l;
	s = l/3+ s;
	kil = l << 2;
	kil = kil + s % 7 + 5|l;
	s = s+ l & kil;
	kil = kil >> 1;
	l = l^kil;
}

