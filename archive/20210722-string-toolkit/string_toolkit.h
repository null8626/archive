#ifdef __cplusplus
#error Please use string_toolkit.hpp.
#endif

#ifndef STRING_TOOLKIT_H
#define STRING_TOOLKIT_H

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define st_is_alnum(t) \
    ((t > 47 && t < 58) || (t > 96 && t < 123) || (t > 64 && t < 91) || t == 95)
#define st_is_num(t) \
    (t > 47 && t < 58)

#ifdef UNICODE
#define st_is_space(t) \
    ((t > 8 && t < 14) || t == 32 || t == 133 || t == 160 || t == 5760 || (t > 8191 && t < 8203) || (t > 8231 && t < 8234) || t == 8239 || t == 8287 || t == 12288)

#define st_char       wchar_t
#define _str_length   wcslen
#define _str_cmp      wcscmp
#else
#define st_is_space(t) \
    ((t > 8 && t < 14) || t == 32 || t == 133 || t == 160)

#define st_char       char
#define _str_length   strlen
#define _str_cmp      strcmp
#endif

#define _trim_arr(_t, _a, _ns, _na) \
    _na = (_t *)malloc(_ns * sizeof(_t)); for (unsigned long long __i = 0; __i < _ns; __i++) _na[__i] = _a[__i]; free(_a)

#ifndef __STDBOOL_H
#define bool   unsigned char
#define true   1
#define false  0
#endif

static void _free_two_dim_arr(void ** arr, const unsigned long long length) {
    if (!length)
        return;
    
    for (unsigned long long i = 0; i < length; i++)
        free(arr[i]);

    free(arr);
}

static void _reverse_string(st_char * arr) {
    unsigned long long temp;
    unsigned long long start = 0;
    unsigned long long end = _str_length(arr) - 1;
    
    while (start < end) {
        temp = arr[start];  
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

#ifdef _WIN32
#define _ST_FUNC __declspec(dllexport)
#else
#define _ST_FUNC
#endif

_ST_FUNC unsigned long long * st_get_indexes(const st_char * str, const st_char character) {
    if (!str[0])
        return NULL;
    
    unsigned long long * ptrl = malloc(_str_length(str) * sizeof(unsigned long long));
    unsigned long long size = 0;
    
    for (unsigned long long i = 0; i < _str_length(str); i++)
        if (str[i] == character) {
            ptrl[size] = i;
            size++;
        }
    
    if (!size) {
        free(ptrl);
        return NULL;
    }
    
    if (size != _str_length(str)) {
        unsigned long long * trimmed;
        _trim_arr(unsigned long long, ptrl, size, trimmed);
        return trimmed;
    }
    
    return ptrl;
}

_ST_FUNC st_char * st_repeat(const st_char * str, const unsigned long long amount) {    
    if (!amount || !str[0])
        return NULL;
    else if (!str[1]) {
        st_char * ptr = malloc(amount * sizeof(st_char));
        memset(ptr, str[0], amount * sizeof(st_char));
        ptr[amount] = 0;
        
        return ptr;
    }
    
    const unsigned long long length = _str_length(str) * amount;
    st_char * ptr = malloc(length * sizeof(st_char));
    ptr[length] = 0;
    
    for (unsigned long long i = 0; i < length; i += amount)
        for (unsigned long long j = i, k = 0; j < i + amount; j++) {
            ptr[j] = str[k];
            k++;
        }
    
    return ptr;
}

_ST_FUNC bool st_starts_with(const st_char * string, const st_char * with_what) {
    const unsigned long long length = _str_length(with_what);
    if (length > _str_length(string))
        return false;
    else if (!with_what[0] || !string[0])
        return true;
    
    for (unsigned long long i = 0; i < length; i++)
        if (string[i] != with_what[i])
            return false;
    
    return true;
}

_ST_FUNC bool st_ends_with(st_char * string, st_char * with_what) {
    _reverse_string(string);
    _reverse_string(with_what);
    return st_starts_with(string, with_what);
}

_ST_FUNC st_char ** st_split(const st_char * string, const st_char delimiter, unsigned long long * size) {
    const unsigned long long string_length = _str_length(string);
    if (string_length < 3) {
        *size = 0;
        return NULL;
    }
    
    unsigned long long ** indices = malloc(string_length * sizeof(unsigned long long *));
    unsigned long long indices_length = 0;
    bool start_match = false;
    
    long long current_start = -1;
    
    for (unsigned long long i = 0; i < string_length; i++) {
        if (!start_match) {
            if (string[i] != delimiter) {
                start_match = true;
                
                if (string[i + 1] == delimiter) {
                    indices[indices_length] = malloc(2 * sizeof(unsigned long long));
                    indices[indices_length][0] = i;
                    indices[indices_length][1] = i;
                    
                    indices_length++;
                } else
                    current_start = i;
            }
            
            continue;
        }
        
        if (i != (string_length - 1)) {
            if (string[i] == delimiter && string[i + 1] != delimiter)
                current_start = i + 1;
        
            else if (string[i] != delimiter && string[i + 1] == delimiter) {
                indices[indices_length] = malloc(2 * sizeof(unsigned long long));
                indices[indices_length][0] = current_start == -1 ? i : (unsigned long long)current_start;
                indices[indices_length][1] = i;
                indices_length++;
                
                current_start = -1;
            }
        
            continue;
        }
        
        if (current_start != -1) {
            indices[indices_length] = malloc(2 * sizeof(unsigned long long));
            indices[indices_length][0] = (unsigned long long)current_start;
            indices[indices_length][1] = string_length - 1;
            indices_length++;
        }
    }
    
    if (indices_length < 2) {
        _free_two_dim_arr((void **)indices, indices_length);
        *size = 0;
        return NULL;
    }
    
    st_char ** output = malloc(indices_length * sizeof(st_char *));
    *size = indices_length;
    
    for (unsigned long long i = 0; i < indices_length; i++) {
        if (indices[i][0] == indices[i][1]) {
            output[i] = malloc(2 * sizeof(st_char));
            output[i][0] = string[indices[i][0]];
            output[i][1] = 0;
            continue;
        }
        
        output[i] = malloc((indices[i][1] - indices[i][0] + 1) * sizeof(st_char));
        
        unsigned long long k = 0;
        for (unsigned long long j = indices[i][0]; j <= indices[i][1]; j++) {
            output[i][k] = string[j];
            k++;
        }
        
        output[i][k] = 0;
    }
    
    _free_two_dim_arr((void **)indices, indices_length);
    return output;
}

_ST_FUNC st_char ** st_to_chunks(const st_char * string, const unsigned long long chunk_by, unsigned long long * size) {
    const unsigned long long string_length = _str_length(string);
    if (chunk_by >= string_length) {
        *size = 0;
        return NULL;
    }
    
    const unsigned long long array_length = ceil((double)string_length / (double)chunk_by);
    st_char ** output = malloc(array_length * sizeof(st_char *));
    *size = array_length;
    
    unsigned long long j = 0;
    for (unsigned long long i = 0; i < array_length; i++) {
        // won't overload
        if ((j + chunk_by) <= string_length) {
            output[i] = malloc(chunk_by * sizeof(st_char));
            output[i][chunk_by] = 0;
            
            const unsigned long long l = j + chunk_by;
            for (unsigned long long k = 0; j < l; j++) {
                output[i][k] = string[j];
                k++;
            }
            
            continue;
        }
        
        // will overload
        const unsigned long long l = string_length - j;
        output[i] = malloc(l * sizeof(st_char));
        output[i][l] = 0;
        
        for (unsigned long long k = 0; j < string_length; j++) {
            output[i][k] = string[j];
            k++;
        }
    }
    
    return output;
}

_ST_FUNC st_char * st_strip(const st_char * string, const st_char strip_char) {
    if (!string[0])
        return NULL;
    
    const unsigned long long length = _str_length(string);
    unsigned long long start = 0;
    unsigned long long end = length - 1;
    
    for (unsigned long long i = 0; i < length; i++)
        if (string[i] != strip_char) {
            start = i;
            break;
        }
    
    for (unsigned long long i = length - 1; i; i--)
        if (string[i] != strip_char) {
            end = i;
            break;
        }
    
    if (!start && end == (length - 1))
        return NULL;
    
    st_char * ptr = malloc((end - start + 1) * sizeof(st_char));
    ptr[end] = 0;
    
    for (unsigned long long i = start, index = 0; i < end; i++) {
        ptr[index] = string[i];
        index++;
    }
    
    return ptr;
}

_ST_FUNC st_char * st_substr(const st_char * string, unsigned long long a, unsigned long long b) {
    if (!string[0] || (a + b) > _str_length(string))
        return NULL;
    
    if (!b)
        b = _str_length(string) - a;
    
    st_char * ptr = malloc(b * sizeof(st_char));
    
    const unsigned long long max = a + b;
    unsigned long long i = 0;
    for (; a < max; a++) {
        ptr[i] = string[a];
        i++;
    }
    
    ptr[i] = 0;
    return ptr;
}

_ST_FUNC st_char * st_get_content_no_flags(const unsigned long long argc, st_char ** argv) {
    if (argc < 2 || (_str_length(argv[0]) > 2 && argv[0][0] == 45 && argv[0][1] == 45))
        return NULL;
    
    unsigned long long * indices = malloc(argc * sizeof(unsigned long long));
    unsigned long long indices_length = 0;
    unsigned long long size = 0;
    
    for (unsigned long long i = 1; i < argc; i++) {
        if (_str_length(argv[i]) > 2 && argv[i][0] == 45 && argv[i][1] == 45)
            continue;
        
        indices[indices_length] = i;
        indices_length++;
        
        size += (_str_length(argv[i]) + 1) * sizeof(st_char);
    }
    
    if (!size) {
        free(indices);
        return NULL;
    }
    
    st_char * ptr = malloc(size);
    ptr[(size / sizeof(st_char)) - 1] = 0;
    
    for (unsigned long long s = 0, index = 0; s < indices_length; s++) {
        for (unsigned long long j = 0; j < _str_length(argv[indices[s]]); j++) {
            ptr[index] = argv[indices[s]][j];
            index++;
        }
        
        if (s != (indices_length - 1)) {
            ptr[index] = 32;
            index++;
        }
    }
    
    free(indices);
    return ptr;
}

_ST_FUNC st_char * st_get_content_no_options(const unsigned long long argc, st_char ** argv) {
    if (argc < 2 || (_str_length(argv[0]) > 2 && argv[0][0] == 45 && argv[0][1] == 45))
        return NULL;
    
    unsigned long long end = argc;
    unsigned long long size = 0;
    for (unsigned long long i = 1; i < argc; i++) {
        if (_str_length(argv[i]) > 2 && argv[i][0] == 45 && argv[i][1] == 45) {
            end = i;
            break;
        }
        
        size += (_str_length(argv[i]) + 1) * sizeof(st_char);
    }
    
    st_char * ptr = malloc(size);
    ptr[(size / sizeof(st_char)) - 1] = 0;
    
    for (unsigned long long start = 1, index = 0; start < end; start++) {
        for (unsigned long long j = 0; j < _str_length(argv[start]); j++) {
            ptr[index] = argv[start][j];
            index++;
        }
        
        if (start != (end - 1)) {
            ptr[index] = 32;
            index++;
        }
    }
    
    return ptr;
}

_ST_FUNC st_char * st_flag_get_value(
    const unsigned long long argc, st_char ** argv, const st_char * key,
    bool * found, bool * has_value) {
    if (!argc)
        return NULL;

    st_char * key_ptr = malloc((2 + _str_length(key)) * sizeof(st_char));
    memset(key_ptr, 45, 2 * sizeof(st_char));
    
    for (unsigned long long i = 0; i < _str_length(key); i++)
        key_ptr[i + 2] = key[i];
    key_ptr[_str_length(key) + 2] = 0;
    
    bool found_ = false;
    
    unsigned long long start = 0;
    unsigned long long end = argc;
    unsigned long long size = 0;
    
    for (unsigned long long i = 0; i < argc; i++) {
        if (!found_ && !_str_cmp(argv[i], key_ptr)) {
            start = i + 1;
            found_ = true;
            continue;
        }
        
        if (found_) {
            if (_str_length(argv[i]) > 2 && argv[i][0] == 45 && argv[i][1] == 45) {
                end = i;
                break;
            }
            
            size += (1 + _str_length(argv[i])) * sizeof(st_char);
        }
    }
    
    if (found != NULL)
        *found = found_;
    
    if (has_value != NULL)
        *has_value = size != 0;

    if (found_ && size) {
        st_char * output = malloc(size);
        output[size / sizeof(st_char)] = 0;
        
        unsigned long long index = 0;
        for (; start != end; start++) {
            for (unsigned long long j = 0; j < _str_length(argv[start]); j++) {
                output[index] = argv[start][j];
                index++;
            }
            
            if (start != (end - 1)) {
                output[index] = 32;
                index++;
            }
        }
        
        return output;
    }
    
    return NULL;
}

_ST_FUNC st_char * st_shorten(const st_char * string, const unsigned long long limit, const st_char * placeholder) {
    if (!string[0] || !limit || _str_length(string) <= limit)
        return NULL;
    
    const unsigned long long placeholder_length = placeholder == NULL ? 3 : _str_length(placeholder);
    st_char * ptr = malloc((1 + limit + placeholder_length) * sizeof(st_char));
    for (unsigned long long i = 0; i < limit; i++)
        ptr[i] = string[i];
    
    if (placeholder != NULL) {
        for (unsigned long long i = 0; i < placeholder_length; i++)
            ptr[placeholder_length + i] = placeholder[i];
    } else
        memset(ptr + limit, 46, 3 * sizeof(st_char));
    
    ptr[limit + placeholder_length] = 0;
    return ptr;
}

// like strcat but you don't have to include a fixed string length
_ST_FUNC st_char * st_dynamic_concat(const unsigned long long amount, ...) {    
    unsigned long long bytes = 1 * sizeof(st_char);
    va_list vl1;
    va_start(vl1, amount);
    
    for (unsigned long long i = 0; i < amount; i++) {
        const st_char * arg = va_arg(vl1, const st_char *);
        if (!arg[0])
            return NULL;
        bytes += _str_length(arg) * sizeof(st_char);
    }
    
    va_end(vl1);
    
    st_char * ptr = malloc(bytes);
    
    va_list vl2;
    va_start(vl2, amount);
    
    unsigned long long index = 0;
    for (unsigned long long i = 0; i < amount; i++) {
        const st_char * arg = va_arg(vl2, const st_char *);
        const unsigned long long argl = _str_length(arg);
        
        for (unsigned long long j = index; j < index + argl; j++)
            ptr[j] = arg[j - index];
        index += argl;
    }
    
    va_end(vl2);
    ptr[index] = 0;
    return ptr;
}

_ST_FUNC bool st_has_custom_emoji(st_char text[]) {
    if (_str_length(text) < 23)
        return false;
    
    unsigned char s = 0; // status
    unsigned char nc = 0; // number count
    bool ia = false; // is animated
    
    for (unsigned long long i = 0; i < _str_length(text); i++) {
        if (!s && text[i] == 60)
            s++;
        else if (s == 1) {
            if (text[i] == 97 && !ia) {
                ia = true;
                continue;
            } else if ((ia && text[i] == 97) || text[i] != 58) return false;
            s++;
        } else if (s == 2) {
            if (st_is_alnum(text[i]))
                nc++;
            else {
                if (text[i] != 58 || nc < 2 || nc > 32)
                    return false;
                nc = 0;
                s++;
            }
        } else if (s == 3) {
            if (st_is_num(text[i])) 
                nc++;
            else
                return text[i] == 62 && nc > 16 && nc < 20;
        }
    }
    
    return false;
}

_ST_FUNC void st_proper_case(st_char text[]) {
    if (!text[0])
        return;
    
    bool s = false;
    unsigned long long i = 0;
    
    if (!st_is_space(text[0])) {
        text[0] = toupper(text[0]);
        i++;
    }
    
    for (; i < _str_length(text); i++) {
        if (st_is_space(text[i])) {
            s = true;
            continue;
        } else if (s) {
            text[i] = toupper(text[i]);
            s = false;
        } else
            text[i] = tolower(text[i]);
    }
}

_ST_FUNC void st_mock(st_char text[]) {
    if (!text[0])
        return;
    
    for (unsigned long long i = 0; i < _str_length(text); i += 2)
        text[i] = toupper(text[i]);
}

_ST_FUNC void st_scramble(st_char text[]) {
    if (!text[0])
        return;
    
    const unsigned long long length = _str_length(text);
    if (length == 1)
        return;
    
    for (unsigned long long i = 0; i < length - 1; i++) {
        const unsigned long long j = rand() % length;
        const st_char temp = text[j];
        text[j] = text[i];
        text[i] = temp;
    }
}

_ST_FUNC st_char * st_progress_bar(
    const double in_total, const double total,
    const st_char elapsed_char, const st_char progress_char,
    const st_char empty_char, const unsigned long long bar_length
) {
    if (in_total < 0 || total < 1 || in_total > total || !bar_length)
        return NULL;
    
    const unsigned long long available = floor((in_total / total) * bar_length);
    const unsigned long long remaining_length = bar_length - (available + (available == bar_length ? 0 : 1));

    st_char * result = malloc(bar_length * sizeof(st_char));
    result[bar_length] = 0;
    
    memset(result, elapsed_char, available * sizeof(st_char));
    
    if (remaining_length) {
        memset(result + available, progress_char, sizeof(st_char));
        memset(result + available + 1, empty_char, remaining_length * sizeof(st_char));
    }
    
    if (_str_length(result) > bar_length) {
        st_char * new_ptr;
        _trim_arr(st_char, result, bar_length, new_ptr);
        return new_ptr;
    }
    
    return result;
}

_ST_FUNC st_char * st_to_abbreviation(const st_char * text) {
    if (_str_length(text) < 2)
        return NULL;
    
    st_char * result = malloc(sizeof(text));
    bool s = false;
    unsigned long long length = 0;
    
    if (!st_is_space(text[0])) {
        result[0] = text[0];
        length++;
    }
    
    for (unsigned long long i = 0; i < _str_length(text); i++) {
        if (st_is_space(text[i])) {
            s = true;
            continue;
        } else if (s) {
            result[length] = text[i];
            length++;
        }
        
        s = false;
    }
    
    result[length] = 0;
    return result;
}

#ifndef __STDBOOL_H
#undef bool
#undef true
#undef false
#endif

#undef _ST_FUNC
#undef _trim_arr
#undef _str_length
#undef _str_cmp
#endif