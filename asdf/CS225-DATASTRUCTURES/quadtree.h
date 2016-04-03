// **************************************************************
// *		   
// *  quadtree.h
// *		   
// *  Quadtree class
// *		   
// *  CS 225 Spring 2008
// *		   
// **************************************************************

#ifndef QUADTREE_H
#define QUADTREE_H

#include "png.h"
#include "math.h"

class Quadtree
{
	public:
	
    Quadtree();
    Quadtree(const PNG & source, int resolution);
    Quadtree(Quadtree const & other);
    ~Quadtree();
    Quadtree const & operator=(Quadtree const & other);
    void buildTree(PNG const & source, int resolution);
    RGBAPixel getPixel(int x, int y) const;
    PNG decompress() const;
    void clockwiseRotate();
    void prune(int tolerance);
    int pruneSize(int tolerance) const;
    int idealPrune (int numLeaves) const;
	
	private:

	// A simple class representing a single node of a Quadtree.
	// You may want to add to this class; in particular, it could probably
	// use a constructor or two...
	class QuadtreeNode
	{
		public:
		QuadtreeNode* nwChild;  // pointer to northwest child
		QuadtreeNode* neChild;  // pointer to northeast child
		QuadtreeNode* swChild;  // pointer to southwest child
		QuadtreeNode* seChild;  // pointer to southeast child

		RGBAPixel element;  // the pixel stored as this node's "data"
	};
	
	QuadtreeNode* root;    // pointer to root of quadtree
	
	
	
	/**** Functions added for testing/grading                ****/
	/**** Do not remove this line or copy its contents here! ****/
	#include "quadtree_given.h"

    // resolution
    size_t res;

    int size;

    //recursive free helper
    void clearH(QuadtreeNode::QuadtreeNode * subRoot) const;

    //recursive helper copy 
    Quadtree::QuadtreeNode * copy(const Quadtree::QuadtreeNode * subRoot) const;

    //recursive build function
    //builds the quadtree of the source image
    QuadtreeNode * build(const PNG & source, int x, int y, int d) const;

    //recursive get function
    //gets the pixel located at x, y of a tree at given root
    RGBAPixel get(QuadtreeNode * subRoot, int x, int y, int dx, int dy, int d) const;

    //recursive decomp function
    //decompresses the tree rooted at the given pointer

    void decomp(QuadtreeNode * ptr, PNG & img, int x, int y, int d) const;
    bool isPrunable(QuadtreeNode * temp, int tolerance, QuadtreeNode * avg) const;
    void clockwiseHelper(QuadtreeNode * temp) const;
    void pruneHelper(QuadtreeNode * temp, int tolerance) const;
    int difference(RGBAPixel px1, RGBAPixel px2) const;
    int pruneSizeHelper(QuadtreeNode * input, int tolerance) const;

};

#endif