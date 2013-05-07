import string, os, sys

# IMPORTANT NOTE: Because D[(0, 0)] must always be zero, in the cost dictionary
# string indexing will begin at 1, loathe as I am to do it.

vowels = ['a','e','i','o','u']

# For strings A and B, D[i][j] is the min cost between A[0:i-1] and B[0:j-1]
D = {}
# Like D, but using a new cost scheme for finding longest common sequences
C = {}

# Find the gamma cost of the string operation from char a to char b
# Deletion or insertion costs 1
# (v -> v) = 0.5, (c -> c) = 0.6, (v -> c) = 1.2, (a == b) = 0
def gamma(a, b):
    if a == b:
        return 0.0
    elif a == '' or b == '':
        return 1.0
    elif a in vowels:
        if b in vowels:
            return 0.5
        else:
            return 1.2
    elif a not in vowels:
        if b not in vowels:
            return 0.6
        else:
            return 1.2


# For finding longest common subsequences,
# defines new costs for string edit 
def sub_gamma(a, b):
    if a == b:
        return 0
    elif (a == '') or (b == ''):
        return 100
    else:
        return 50000


# Given strings A and B, find D(i,j) for all 0 <= i <= |A|, 0 <= j <= |B|
# AKA "Algorithm X"
# store results in given dictionary d
# Mode determines which cost assignment function is used.
# 0 = 'gamma' (for regular string-edit distance), 1 = 'sub_gamma' for (longest subsequence)
def get_D(A, B, d, mode):
    l_A = len(A) 
    l_B = len(B)
    d[(0, 0)] = 0

    # Set all D[i][0] values to D[i-1][0] plus cost of deletion
    for i in range(l_A):
        i += 1
        if mode == 0:
            cost = gamma(A[i-1], '')
        else:
            cost = sub_gamma(A[i-1], '')
        d[(i, 0)] = d[(i-1, 0)] + cost

    # Set all D[0][j] values to D[0][j-1] plus cost of insertion
    for j in range(l_B):
        j += 1
        if mode == 0:
            cost = gamma('', B[j-1])
        else:
            cost = sub_gamma('', B[j-1])             
        d[(0, j)] = d[(0, j-1)] + cost

    # For every non-edge i and j, find D[i][j] based on Theorem 2:
    # D[i][j] is the minimum of 3 cases, 
    # 1: A[i-1] and B[j-1] are associated (joined in the trace)
    # 2: A[i-1] is deleted
    # 3: B[j-1] is inserted
    for i in range(l_A):
        i += 1
        for j in range(l_B):
            j += 1
            if mode == 0:
                m1 = d[(i-1, j-1)] + gamma(A[i-1], B[j-1])
                m2 = d[(i-1, j)] + gamma(A[i-1], '')
                m3 = d[(i, j-1)] + gamma('', B[j-1])
            else:
                m1 = d[(i-1, j-1)] + sub_gamma(A[i-1], B[j-1])
                m2 = d[(i-1, j)] + sub_gamma(A[i-1], '')
                m3 = d[(i, j-1)] + sub_gamma('', B[j-1])
            d[(i, j)] = min(m1, m2, m3)


# Given least cost dictionary d for strings A and B,
# T is the trace of least cost between them.
# Print alignment of T and its pairs.
# Last argument sets 'mode', (regular string-edit distance) or (longest subsequence)
# ...not the most elegant, I know. 
def trace(A, B, d, mode):
    get_D(A, B, d, mode)
    Ai = []
    Bj = []
    t = []
    i = len(A)
    j = len(B)
    while (i != 0) & (j != 0):
        # Assign cost value based on mode
        if mode == 0:
            cost_A = gamma(A[i-1], '')
            cost_B = gamma('', B[j-1])
        else:
            cost_A = sub_gamma(A[i-1], '')
            cost_B = sub_gamma('', B[j-1])
        
        # Get trace
        if d[(i, j)] == d[(i-1, j)] + cost_A:
            Ai.append(A[i-1])
            Bj.append(' ')
            i -= 1
        elif d[(i, j)] == d[(i, j-1)] + cost_B:
            Ai.append(' ')
            Bj.append(B[j-1])
            j -= 1
        else:
            Ai.append(A[i-1])
            Bj.append(B[j-1])
            t.append((i-1, j-1))
            i -= 1
            j -= 1

    # If either i or j are still > 0, there are letters unaccounted for, add them to list.
    while i != 0:
        Ai.append(A[i-1])
        Bj.append(' ')
        i -= 1
    while j != 0:
        Ai.append(' ')
        Bj.append(B[j-1])
        j -= 1

    # Reverse all the lists, since we are iterating through the string backwards
    Ai.reverse()
    Bj.reverse()
    t.reverse()

    # If distance mode,
    if mode == 0:
        print 'String-edit distance is ' + str(d[len(A), len(B)]) + '.\n\n'
        print 'Trace:'
        print t
        print ''
        
        # Print A
        for c in Ai:
            print c,
        print ''

        # Print B
        for c in Bj:
            print c,
        print ''

    # If longest common substring mode
    if mode == 1:
        # Start saving common strings in com1 and com2, comparing to get the longest common string
        com1 = []
        com2 = []
        # Determine which list to write to, 0 = com1 or 1 = com2
        tog = 0
        # Find common subsequence
        for i, pair in enumerate(t):
            a = A[pair[0]]
            b = B[pair[1]]
            if a == b:
                if tog == 0:
                    com1.append(A[pair[0]])
                else:
                    com2.append(A[pair[0]])
                # If the next entry in the trace is not consecutive in the string, 
                # 'toggle', change tog value to write to whichever com list is
                # storing a shorter string
                if i+1 < len(t):
                    if t[i+1][0] != pair[0]+1:
                        if len(com1) > len(com2):
                            tog = 1
                            com2 = []
                        else:
                            tog = 0
                            com1 = []

        # Get the longest saved substring
        if len(com1) > len(com2):
            longest = com1
        else:
            longest = com2
        if len(longest) > 0:
            res = ''.join(longest)
            print 'Longest common subsequence is',
            print '"' + res + '", length ' + str(len(res)) + '.'
        else:
            print 'No subsequences in common.'
        

# Main! Get user to input two string, find string-edit distance between them.
# Print best trace
def main():
    # Get strings
    s1 = raw_input('String 1: ')
    s2 = raw_input('String 2: ')

    # Get/print distance and trace 
    print ''
    trace(s1, s2, D, 0)
    print ''
    # Longest common subsequence
    trace(s1, s2, C, 1)
    print ''

if __name__ == '__main__':
    main()


