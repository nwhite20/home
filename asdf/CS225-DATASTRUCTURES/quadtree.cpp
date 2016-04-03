// **************************************************************
// *		   
// *  quadtree.cpp
// *		   
// *  Quadtree class
// *		   
// *  CS 225 Spring 2008
// *		   
// **************************************************************

#include "quadtree.h"

using namespace std;

// produces empty quadtree

Quadtree::Quadtree()
{
    root = NULL;
    res = 0;
    size = 0;
}


//the two argument constructor, produces a quadtree of the upper left
//d by d block of the given source image

Quadtree::Quadtree(const PNG & source, int resolution)
{    
    root = NULL;
    res = resolution;
    size = res;
    buildTree(source,resolution);
}

//the copy constructor
Quadtree::Quadtree(Quadtree const & other)
{
    size = other.size;
    
    //recursive helper function necessary (copy)
    root = copy(other.root);
}

//destructor
//
Quadtree::~Quadtree()
{
    
    clearH(root);
    root = NULL;
}

//clear
void Quadtree::clearH(Quadtree::QuadtreeNode * subRoot) const
{
	//base
	if (subRoot == NULL)
		return;

	//recursive cases
	clearH(subRoot->neChild);
	clearH(subRoot->nwChild);
	clearH(subRoot->seChild);
	clearH(subRoot->swChild);

	delete subRoot;

}

//assignment operator
Quadtree const & Quadtree::operator=(Quadtree const & other)
{
    //free the space used by the original tree 

    clearH(root);
	
    root = copy(other.root);

    size = other.size;

    return *this;
}

//copy helper function
//makes a copy of the tree with the given root pointer

Quadtree::QuadtreeNode * Quadtree::copy(const Quadtree::QuadtreeNode * subRoot) const
{
	
    //basecase
    if (subRoot == NULL)
    	return NULL;
   	
   	//recursive copying
   	else{
   	QuadtreeNode * ret = new QuadtreeNode;
   	ret->element = subRoot->element;
    ret->seChild = copy(subRoot->seChild);
    ret->neChild = copy(subRoot->neChild);
    ret->swChild = copy(subRoot->swChild);
    ret->nwChild = copy(subRoot->nwChild);

    return ret;
    }
    
    
}

//buildtree function
//

void Quadtree::buildTree(PNG const & source, int resolution)
{
    //free the space used by the original tree
    
    clearH(root);
    
    size = resolution;
    
    root = build(source, 0, 0, size);
    
    
}

//build recursive function
//builds quadtree of the source img

Quadtree::QuadtreeNode * Quadtree::build(const PNG & source, int x, int y, int d) const
{
    //leaf
    if (d == 1)
    {
    	QuadtreeNode * ret = new QuadtreeNode;
    	ret->element = *source(x,y);
    	ret->neChild = ret->nwChild = ret->swChild = ret->seChild = NULL;
    	
    	return ret;
    }
    
    //average node
    
    QuadtreeNode * ret1 = new QuadtreeNode;
    

    ret1->nwChild = build(source, x, y, d/2);
    ret1->neChild = build(source,x+d/2,y,d/2);
    ret1->swChild = build(source,x,y+d/2,d/2);
    ret1->seChild = build(source,x+d/2,y+d/2,d/2);
    
    //averaging
    ret1->element.red = (ret1->nwChild->element.red + ret1->neChild->element.red + ret1->seChild->element.red + ret1->swChild->element.red) / 4;
    ret1->element.blue = (ret1->nwChild->element.blue + ret1->neChild->element.blue + ret1->seChild->element.blue + ret1->swChild->element.blue) / 4;
    ret1->element.green = (ret1->nwChild->element.green + ret1->neChild->element.green + ret1->seChild->element.green + ret1->swChild->element.green) / 4;
    
    return ret1;
}

//getPixel fuction
//returns the RGBAPixel at the given coordinates

RGBAPixel Quadtree::getPixel(int x, int y) const
{
    //error checking
    if (x>=size || y >= size || size == 0)
    {
        return RGBAPixel();
    }

    //get the pixel
    return get(root, x, y, 0, 0, size);
}

//get recursive pixel getter

RGBAPixel Quadtree::get(QuadtreeNode * subRoot, int x, int y, int dx, int dy, int d) const
{
    //in case
    if (subRoot == NULL)
    	{
    		return RGBAPixel();
    	}
    	
    //leaf
    if (subRoot->neChild == NULL || subRoot->nwChild == NULL || subRoot->seChild == NULL || subRoot->swChild == NULL)
    {
    	
    	return subRoot->element;
    	
    }
    	
    //we have to determine what quadrant 
     if (x < dx + d/2 && y < dy + d/2) // NW
    	return get(subRoot->nwChild,x,y,dx,dy,d/2);
    	
    else if (x >= dx+d/2 && y < dy + d/2) // NE
    	return get(subRoot->neChild,x,y,dx+d/2,dy,d/2);
    	
    else if (x < dx + d/2 && y >= dy + d/2) // SW
    	return get(subRoot->swChild,x,y,dx,dy+d/2,d/2);
    	
    else if (x >= dx + d/2 && y >= dy + d/2) // SE
    	return get(subRoot->seChild,x,y,dx+d/2,dy+d/2,d/2);

    return RGBAPixel();
    
}


//decompress function
//returns the underlying PNG object represented by the quadtree

PNG Quadtree::decompress() const
{
    if (size == 0 || root == NULL)
        return PNG();

    PNG ret = PNG(size,size);

    decomp(root, ret, 0, 0, size);

    return ret;
}

//decompress helper function, decomp
//decompresses 
void Quadtree::decomp(QuadtreeNode * subRoot, PNG & img, int x, int y, int d) const
{
	//leaf
	if (subRoot->neChild == NULL || subRoot->nwChild == NULL || subRoot->seChild == NULL || subRoot->swChild == NULL)
	{
		//fill all pixels in the square of size d if we have reached a leaf
		int i = x;
		for (i = x; i < x + d;i++)
        {
		int j = y;
            for(j = y; j < y + d;j++)
            {
                *(img(i,j)) = subRoot->element;
            }
        }
		
		return;
	}
	
	else{
	//recurse until we reach a leaf
	decomp(subRoot->nwChild, img, x,y,d/2);
	decomp(subRoot->neChild, img, x+d/2,y,d/2);
	decomp(subRoot->swChild, img, x,y+d/2,d/2);
	decomp(subRoot->seChild, img, x+d/2,y+d/2,d/2);
	}
}



//prune function
//
//compresses the quadtree image 

void Quadtree::prune(int tolerance)
{
    pruneHelper(root, tolerance);
}

void Quadtree::pruneHelper(QuadtreeNode * temp, int tolerance) const
{
    //base case
    if (temp == NULL)
        return;
    
    //check if the node you are at is prunable
    if (isPrunable(temp, tolerance, temp))
    {

        clearH(temp->seChild);
        clearH(temp->swChild);
        clearH(temp->nwChild);
        clearH(temp->neChild);
        
        temp->nwChild = NULL;
        temp->neChild = NULL;
        temp->seChild = NULL;
        temp->swChild = NULL;
        return;
    } 
    
    pruneHelper(temp->nwChild, tolerance);
    pruneHelper(temp->neChild, tolerance);
    pruneHelper(temp->seChild, tolerance);
    pruneHelper(temp->swChild, tolerance);


}

//isPrunable function
//checks to see if a given node is prunable

bool Quadtree::isPrunable(QuadtreeNode * temp, int tolerance, QuadtreeNode * avg) const
{
    if (temp == NULL)
        return true;

    //if not  at a leaf, recurse
    if (temp->nwChild != NULL)
    {
        return (isPrunable(temp->nwChild, tolerance, avg) && isPrunable(temp->neChild, tolerance, avg) &&
                isPrunable(temp->swChild, tolerance, avg) && isPrunable(temp->seChild, tolerance, avg));
    }
    
    //if difference is too great
    
    if (difference(temp->element, avg->element) > tolerance)
          return false;

    //otherwise small
    return true;
}

//difference function for RGBAPixels
int Quadtree::difference(RGBAPixel px1, RGBAPixel px2) const
{
    int ret = 0;
    ret += pow(px1.red - px2.red, 2);
    ret += pow(px1.green - px2.green, 2);
    ret += pow(px1.blue - px2.blue, 2);
    return ret;
}

//pruneSize fucntion
//returns a count of the total number of leaves the quadtree would have
//if it were pruned as in the prune function
int Quadtree::pruneSize(int tolerance) const
{
    return pruneSizeHelper(root, tolerance);
}

//pruneSizeHelper function
//takes in a node, determines the number of leaves that node would have if they were all pruned

int Quadtree::pruneSizeHelper(QuadtreeNode * input, int tolerance) const
{
	if (input == NULL)
		return 0;
		
	//base case, reached a leaf, add a leaf to the total
    if (input->nwChild == NULL || isPrunable(input, tolerance, input))
    	return 1;

    return (pruneSizeHelper(input->nwChild, tolerance) + pruneSizeHelper(input->neChild, tolerance) + 
    		pruneSizeHelper(input->swChild, tolerance) + pruneSizeHelper(input->seChild, tolerance));
}

//idealPrune function

int Quadtree::idealPrune(int numLeaves) const
{
    int max = (255*255) + (255*255) + (255*255);
    int min = 0;
    int tol = (min + max) / 2;
    int curLeaves = 0;
    int lastval = max;
    
    
    while ((tol > min) && (tol < max))
    {
    	//avg
        curLeaves = pruneSize(tol);
    	
    	//if curLeaves is no more, set
    	if (curLeaves <= numLeaves)
    	{
    		lastval = tol;
            max = tol;
        }
	
    	//curLeaves is too high
    	else// (curLeaves > numLeaves)
    	{
    		min = tol;
    	}
    	
	tol = (min + max)/2;
    }
    if (lastval == 1)
        lastval=0;
    return lastval;
}


void Quadtree::clockwiseRotate() 
{
	clockwiseHelper(root);
}

void Quadtree::clockwiseHelper(QuadtreeNode* temp) const
{
	
		if (temp == NULL)
			return;
	//else rotate
		
		QuadtreeNode* nw = temp->nwChild;
		QuadtreeNode* ne = temp->neChild;
		QuadtreeNode* sw = temp->swChild;
		QuadtreeNode* se = temp->seChild;
		
		temp->nwChild = sw;
		temp->neChild = nw;
		temp->seChild = ne;
		temp->swChild = se;
		
		clockwiseHelper(nw);
		clockwiseHelper(ne);
		clockwiseHelper(sw);
		clockwiseHelper(se);
}