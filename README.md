# Сравнение алгоритмов кодирования

## Алгоритм Хафмана

Алгоритмическая сложность сжатия: O(n + klogk), где
n - количество символов в файле
k - количество уникальных символов в файле

Алгоритмическая сложность извлечения: O(k + m), где
k - количество уникальных символов в файле
m - количество символов в закодированном файле

## Алгоритм RLE (кодирование повторов)

Алгоритмическая сложность сжатия: O(n), где
n - количество символов в файле

Алгоритмическая сложность извлечения: O(m), где
m - количество символов в закодированном файле

## Примеры

Было выполнено по 3 примера файла для категорий: видео, изображения, текстовый документ

Все подробные вычисления можно посмотреть в [директории](/test/)
