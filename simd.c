static int sum_vectorized(int n, int *a)
{
    // WRITE YOUR VECTORIZED CODE HERE
	int storeSIMDed[4];
    int i;
    int j = n / 4;
    
	__m128i sum = _mm_setzero_si128();
    __m128i	operand; 
    
	for (i = 0; i < j; i++) {
        operand = _mm_loadu_si128((__m128i *) a + i);
        sum = _mm_add_epi32(operand, sum);
    }
    
	_mm_storeu_si128((__m128i *) storeSIMDed, sum);
    
	int sumResult = 0;

	for (i = 0; i < 4; i++) {
		sumResult += storeSIMDed[i];
	}	
	
	for (i = j * 4; i < n; i++) {
        sumResult = sumResult + a[i];
    }

    return sumResult;
}




static int sum_vectorized_unrolled(int n, int *a)
{
    // UNROLL YOUR VECTORIZED CODE HERE
    int storeSIMDed[4];
    int i;
    int j = n / 4;

    __m128i sum = _mm_setzero_si128();
    __m128i operand;
	
    for (i = 0; i < j; i = i + 4) {
        operand = _mm_loadu_si128((__m128i *) a + i);
        sum = _mm_add_epi32(operand, sum);

        operand = _mm_loadu_si128((__m128i *) a + i + 1);
        sum = _mm_add_epi32(operand, sum);

        operand = _mm_loadu_si128((__m128i *) a + i + 2);
        sum = _mm_add_epi32(operand, sum);

        operand = _mm_loadu_si128((__m128i *) a + i + 3);
        sum = _mm_add_epi32(operand, sum);
    }
    
    for (i = j / 4 * 4; i < j; i++) {
        operand = _mm_loadu_si128((__m128i *) a + i);
        sum = _mm_add_epi32(operand, sum);
    }
	
    _mm_storeu_si128((__m128i *) storeSIMDed, sum);

	int sumResult = 0;
	for (i = 0; i < 4; i++) {
		sumResult += storeSIMDed[i];
	}	

    for (i = j * 4; i < n; i++) {
        sumResult = sumResult + a[i];
    }

    return sumResult;
}
