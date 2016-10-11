# Basic color compressor using the k-means algorithm
# Usage: python3 [k_value] [num_iterations] [input_file] [output_file]

from PIL import Image
import numpy as np
import random
import scipy
import sys


# For a given cell, find the nearest centroid. Return its index.
def nearest(cell, centroids):
    best = 100000000
    idx = 0
    for i,c in enumerate(centroids):
        d = np.linalg.norm(cell - c)
        if d < best:
            best = d
            idx = i
    return idx


def main():
    fname = sys.argv[3]
    k = int(sys.argv[1])
    iterations = int(sys.argv[2])
    out_fname = sys.argv[4]

    # Read the image in a three-dimensional numpy array
    image = Image.open(fname)
    pix = np.asarray(image, dtype=np.int)
    n = pix.shape[0]
    
    # Array storing centroids, initialize with random cells
    centroids = np.zeros((k, 3))
    for s in range(k):
        centroids[s] = pix[random.randint(0, n)][random.randint(0, n)]
    
    # Iterate!
    for t in range(iterations):
        print(t)
        # scratch arrays for recomputing centroids
        scratch = np.zeros((k, 3)) # store sums
        counters = np.zeros((k)) # store number of points in cluster

        # Assign clusters, then recompute centroids
        for i in range(n):
            for j in range(n):
                idx = nearest(pix[i][j], centroids)
                scratch[idx] += pix[i][j]
                counters[idx] += 1
        for i in range(k):
            if counters[i] != 0:
                centroids[i] = scratch[i]/counters[i]

    # Create new pixel map
    width = pix.shape[0]
    height = pix.shape[1]
    res = np.zeros((width, height, 3), dtype=np.uint8)
    for i in range(width):
        for j in range(height):
            idx = nearest(pix[i][j], centroids)
            res[i][j] = centroids[idx]
            
    # To save the image corresponding to matrix A
    out_im = Image.fromarray(res, "RGB")
    out_im.save(out_fname)

if __name__ == "__main__":
    main()
