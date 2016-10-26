import string, os, sys
from operator import itemgetter

# Takes two words, returns # overlapping pairs
def overlap(w1, w2):
    counter = 0
    l1 = len(w1)-1
    l2 = len(w2)-1
    # Make list of pair in w2
    check = []
    for i,char in enumerate(w2):
        if i != l2:
            check.append(w2[i] + w2[i+1])
    # For every pair in w1, if pair is in w2, counter + 1
    for i,char in enumerate(w1):
        if i != l1:
            if (w1[i] + w1[i+1]) in check:
                counter += 1
    return counter

# Returns total overlap for a list (set of anagrams), summing overlap 
# for each word with each other word.
def t_overlap(ws):
    l = len(ws)-1
    t = 0
    for i in ws:
        for j in ws:
            if i < j:
                t += overlap(i, j)
    return t
                        
def main():
    # Get dictionary file -> list of words
    f = open('dictionary.txt', 'r')
    words = []
    for line in f:
        tmp = line.replace('\n', '')
        if tmp != '':
            words.append(tmp.lower())

    # Create dictionary -- word entires are stored by their alphabetized letters
    # ex. dic[['a','b']] = ['ab','ba']
    # Each entry contains a set of anagrams as a list
    dic = {}
    keys = []
    # Generate all keys and initialie dic with empty lists
    for w in words:
        keys.append(''.join(sorted(list(w.replace('-', '')))))
    u_keys = set(keys)
    for i in u_keys:
        dic[i] = []
    # Add words to dic
    for w in words:
        k = ''.join(sorted(list(w.replace('-', ''))))
        dic[k].append(w)

    # Get list of anagram sets sorted by length of individual words
    # Commented out for now.
    # anagrams = []
    # for k in dic.keys():
    #     anagrams.append(dic[k])
    # anagrams.sort(key=len)

    # Get all sets of anagrams of length >= 6. 
    # Sort by size (number of words, descending), 
    # then length (length of each word in set, descending),
    # then overlap (ascending)
    tuples = []

    # Get list of tuples(key, size, length, overlap) of all sets >= 6
    for k in dic.keys():
        l = len(dic[k][0])
        e = set(dic[k])
        dic[k] = e
        sz = len(e)
        ov = t_overlap(e)
        if l >= 6 and sz >= 2:
            tuples.append( (k, -1*sz, -1*l, ov) )

    # Sort list of tuples
    tuples.sort(key=itemgetter(1,2,3))

    # Print results
    print 'Anagram sets of length >= 6:'
    print ''
    for i in tuples:
        print dic[i[0]] 


if __name__ == '__main__':
    main()
