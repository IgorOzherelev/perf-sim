int fibonacci(int N)  // рекурсивная функция
{
	  if (N == 1 || N == 2)
		 return 1; // первые 2 числа равны 1
	  return fibonacci(N - 1) + fibonacci(N - 2); // складываем предыдущие 2 числа
}
int main()
{
	int N= 10;
	int s = 0;
	for (int i = 1; i <= N; i++) // В цикле выводим N чисел Фибоначчи
	 	 s+= fibonacci(i);
	return 0;
}
