#ifndef STRING_H
#define STRING_H

int string_length(const char *str)
{
    int len = 0;

    while (str[len] != '\0')
    {
        len++;
    }

    return len;
}

int min(const int num1, const int num2)
{
    return num1 < num2 ? num1 : num2;
}

int string_compare(const char *str1, const char *str2)
{
    int len1 = string_length(str1), len2 = string_length(str2);
    int len = min(len1, len2);

    for (int i = 0; i < len; i++)
    {
        if (str1[i] != str2[i])
        {
            return str1[i] < str2[i] ? -1 : 1;
        }
    }

    if (len1 != len2)
    {
        return len1 < len2 ? -1 : 1;
    }

    return 0;
}

char *string_copy(const char *source)
{
    int len = string_length(source);

    char *destination = (char *)malloc(len + 1);

    for (int i = 0; i < len; i++)
    {
        destination[i] = source[i];
    }

    destination[len] = '\0';

    return destination;
}

#endif