#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct QuadtreeNode {
	unsigned char blue,green,red;
	unsigned int area;
	int top_left, top_right;
	int bottom_left, bottom_right;
}__attribute__((packed)) QuadtreeNode;

typedef struct pixel{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} pixel;

typedef struct nod{
	unsigned char red, green, blue;
	struct nod* top_left;
	struct nod* top_right;
	struct nod* bottom_right;
	struct nod* bottom_left;
} *nod;

nod init() {
	nod new = malloc(sizeof(struct nod));
	new->top_left = NULL;
	new->top_right = NULL;
	new->bottom_right = NULL;
	new->bottom_left = NULL;
	new->red = 0;
	new->green = 0;
	new->blue = 0;
	return new;
}

pixel** image(char *filename, unsigned int *width, unsigned int *height) {

	FILE *f = fopen(filename, "rb");
	char tip[3], c[2];
	unsigned int i, j, max;
	fscanf(f, "%s\n%u%u\n%d%c", tip, width, height, &max, c);

	pixel** grid;
	grid = malloc( (*height) * sizeof(pixel*));

	for (i = 0; i < (*height); i++)
		grid[i] = malloc(*width * sizeof(pixel));

	for(i = 0; i < *height; i++)
		for(j = 0; j < *width; j++) {
            fread(&grid[i][j].red,sizeof(unsigned char),1,f);
            fread(&grid[i][j].green,sizeof(unsigned char),1,f);
            fread(&grid[i][j].blue,sizeof(unsigned char),1,f);
        }

	fclose(f);
	return grid;
}

void freeTree(nod* root) {
	if ((*root) == NULL)
		return;
	freeTree(&(*root)->top_left);
	freeTree(&(*root)->top_right);
	freeTree(&(*root)->bottom_right);
	freeTree(&(*root)->bottom_left);
	free(*root);
}

void inv(nod* nod1, nod* nod2) {
	nod aux = *nod1;
	*nod1 = *nod2;
	*nod2 = aux;
}

void unif(nod nod, pixel **grid, int width, long long size, int x, int y, int factor, unsigned int *nr_noduri, unsigned int *nr_culori) {
	long long r=0, g=0, b=0, sum=0, mean;
	int i, j;

	//pentru fiecare canal RGB, se calculeaza suma si dupa media aritmetica
	for(i = x; i < x+width; i++)
		for(j = y; j < y+width; j++) {
			r+=(int)grid[i][j].red;
			g+=(int)grid[i][j].green;
			b+=(int)grid[i][j].blue;
		}
	r = r/size;
	g = g/size;
	b = b/size;

	nod->red = r;
	nod->green = g;
	nod->blue = b;

	//folosing formula, aflu daca nodul curent este terminal sau neterminal
	for (i = x; i < x+width; i++)
		for (j = y; j < y+width; j++)
			sum += pow((r-(int)grid[i][j].red), 2) + pow((g-(int)grid[i][j].green), 2) + pow((b-(int)grid[i][j].blue), 2);
	mean = sum / (3 * size);

	if (mean <= factor)
		(*nr_culori)++;
    //daca nodul este neterminal, se mai imparte din nou in 4 parti
	else {
        	nod->top_left = init();
        	nod->top_right = init();
        	nod->bottom_right = init();
        	nod->bottom_left = init();

            (*nr_noduri) += 4;
            unif(nod->top_left, grid, width/2, size/4, x, y, factor, nr_noduri, nr_culori);
            unif(nod->top_right, grid, width/2, size/4,  x, y + width/2, factor, nr_noduri, nr_culori);
            unif(nod->bottom_right, grid, width/2, size/4, x + width/2, y + width/2, factor, nr_noduri, nr_culori);
            unif(nod->bottom_left, grid, width/2, size/4, x + width/2, y, factor, nr_noduri, nr_culori);
	}
}

void horiz(nod* root) {
	if ((*root)->top_left != NULL) { //daca nu e nod frunza
		//se interschimba blocul din stanga sus cu cel din dreapta sus
		inv(&((*root)->top_left), &((*root)->top_right));
		inv(&((*root)->bottom_left), &((*root)->bottom_right));
		//apeleaza functia pentru fii
		horiz(&((*root)->top_left));
		horiz(&((*root)->top_right));
		horiz(&((*root)->bottom_left));
		horiz(&((*root)->bottom_right));
	}
}

void vert(nod* root) {
	if ((*root)->top_left != NULL) { //daca nu e nod frunza
		//se interschimba blocul din stanga sus cu cel din dreapta sus
		inv(&((*root)->bottom_left), &((*root)->top_left));
		inv(&((*root)->bottom_right), &((*root)->top_right));
		//apeleaza functia pentru fii
		vert(&((*root)->top_left));
		vert(&((*root)->top_right));
		vert(&((*root)->bottom_left));
		vert(&((*root)->bottom_right));
	}
}

//scriu elementele vectorului
void vector(QuadtreeNode *v, nod root, unsigned int area, int *k) {
	v[*k].red = root->red;
	v[*k].green = root->green;
	v[*k].blue = root->blue;
	v[*k].area = area;

	//pentru nodul care nu e frunza, repet functia pentru fiii sai
	if (root->top_left != NULL) {
		int aux = (*k);
		area = area/4;
		(*k)++;
		v[aux].top_left = (*k);
		vector(v, root->top_left, area, k);
		(*k)++;
		v[aux].top_right = (*k);
		vector(v, root->top_right, area, k);
		(*k)++;
		v[aux].bottom_right = (*k);
		vector(v, root->bottom_right, area, k);
		(*k)++;
		v[aux].bottom_left = (*k);
		vector(v, root->bottom_left, area, k);
	}

	//daca e frunza, atunci indicele asociat nodului copil devine -1
	else {
		v[*k].top_left = -1;
		v[*k].top_right = -1;
		v[*k].bottom_right = -1;
		v[*k].bottom_left = -1;
	}
}

void makegrid(pixel **grid, nod root, int x, int y, unsigned int width) {

	int i, j;

	for (i = x; i < x+width; i++)
		for (j = y; j < y+width; j++) {
			grid[i][j].red = root->red;
			grid[i][j].green = root->green;
			grid[i][j].blue = root->blue;
		}

	if (root->top_left != NULL) {
        width = width/2;
		makegrid(grid, root->top_left, x, y, width);
		makegrid(grid, root->top_right, x, y+width, width);
		makegrid(grid, root->bottom_left, x+width, y, width);
		makegrid(grid, root->bottom_right, x+width, y+width, width);
	}
}

void compress(nod* root, int factor, char *file, char *out) {

	unsigned int width, height, i, numar_culori = 0, numar_noduri = 5;
	pixel **grid = image(file, &width, &height);
	long long size = width*height;

	unif((*root)->top_left, grid, width/2, size/4, 0, 0, factor, &numar_noduri, &numar_culori);
	unif((*root)->top_right, grid, width/2, size/4, 0, width/2, factor, &numar_noduri, &numar_culori);
	unif((*root)->bottom_right, grid, width/2, size/4, height/2, width/2, factor, &numar_noduri, &numar_culori);
	unif((*root)->bottom_left, grid, width/2, size/4, height/2, 0, factor, &numar_noduri, &numar_culori);


	for (i = 0; i < height; i++)
		free(grid[i]);
	free(grid);

	QuadtreeNode *v;
	v = (QuadtreeNode*)malloc(numar_noduri*sizeof(QuadtreeNode));

	int k = 0;
	vector(v, *root, size, &k);
	freeTree(&(*root));

	FILE *fout = fopen(out, "wb");
	fwrite(&numar_culori, sizeof(unsigned int), 1, fout);
	fwrite(&numar_noduri, sizeof(unsigned int), 1, fout);
	fwrite(v, sizeof(QuadtreeNode), numar_noduri, fout);
	fclose(fout);
	free(v);
}

void mirror(nod* root, char type, int factor, char *file, char *out) {
	unsigned int width, height, i, numar_culori = 0, numar_noduri = 5;
	pixel **grid = image(file, &width, &height);
	long long size = width*height;

	unif((*root)->top_left, grid, width/2, size/4, 0, 0, factor, &numar_noduri, &numar_culori);
	unif((*root)->top_right, grid, width/2, size/4, 0, width/2, factor, &numar_noduri, &numar_culori);
	unif((*root)->bottom_right, grid, width/2, size/4, height/2, width/2, factor, &numar_noduri, &numar_culori);
	unif((*root)->bottom_left, grid, width/2, size/4, height/2, 0, factor, &numar_noduri, &numar_culori);

	if(type == 'h')
		horiz(&(*root));
	if(type == 'v')
        	vert(&(*root));

	makegrid(grid, *root, 0, 0, width);

	freeTree(&(*root));
	FILE *f = fopen(out, "wb");
	fprintf(f, "%s\n", "P6");
	fprintf(f, "%u %u\n", height, width);
	    fprintf(f, "%u\n", 255);

	for (i = 0; i < height; i++)
		fwrite(grid[i], sizeof(pixel), width, f);

	for (i = 0; i < height; i++)
		free(grid[i]);

	free(grid);
	fclose(f);
}

void maketree(nod* root, QuadtreeNode* v, unsigned int nr_noduri, unsigned int i) {
    if(nr_noduri == 0)
	  return;

    (*root)->red = v[i].red;
    (*root)->green = v[i].green;
    (*root)->blue = v[i].blue;

    if (v[i].top_left == -1) {
        (*root)->top_left = NULL;
        (*root)->top_right = NULL;
        (*root)->bottom_right = NULL;
        (*root)->bottom_left = NULL;
    }
    else {
        maketree(&(*root)->top_left, v, nr_noduri-1, v[i].top_left);
        maketree(&(*root)->top_right, v, nr_noduri-1, v[i].top_right);
        maketree(&(*root)->bottom_right, v, nr_noduri-1, v[i].bottom_right);
        maketree(&(*root)->bottom_left, v, nr_noduri-1, v[i].bottom_left);
    }
}

void decompress(char *in, char *out) {

	FILE *fin, *fout;
	fin = fopen(in, "rb");
	unsigned int nr_culori, nr_noduri, i;

	QuadtreeNode *v;
	fread(&nr_culori, sizeof(unsigned int), 1, fin);
	fread(&nr_noduri, sizeof(unsigned int), 1, fin);
	v = malloc(nr_noduri*sizeof(QuadtreeNode));
	fread(v, sizeof(QuadtreeNode), nr_noduri, fin);
	fclose(fin);

	unsigned int width = floor(sqrt(v[0].area));
	unsigned int height = width;

	nod root = NULL;
    root = init();
	maketree(&root, v, nr_noduri, 0);

	free(v);

	pixel **grid;
	grid = malloc(width*sizeof(pixel*));
	for (i = 0; i < width; i++)
		grid[i] = malloc(width*sizeof(pixel));

	makegrid(grid, root, 0, 0, width);

	fout = fopen(out, "wb");
	fprintf(fout, "%s\n%u %u\n%u\n", "P6", width, height, 255);
	for (i = 0; i < height; i++)
		fwrite(grid[i], sizeof(pixel), width, fout);
	for (i = 0; i < height; i++)
		free(grid[i]);
	free(grid);
	fclose(fout);
}

int main(int argc, char **argv)
{
    if(strcmp(argv[1],"-c") == 0) {
        nod root;
        root = NULL;
        root = init();
        root->top_left = init();
        root->top_right = init();
        root->bottom_right = init();
        root->bottom_left = init();

        int factor = atoi(argv[2]);
        compress(&root, factor, argv[3], argv[4]);
    }

    if(strcmp(argv[1],"-m") == 0) {
        nod root;
        root = NULL;
        root = init();
        root->top_left = init();
        root->top_right = init();
        root->bottom_right = init();
        root->bottom_left = init();

        char type = argv[2][0];
        int factor = atoi(argv[3]);
        mirror(&root, type, factor, argv[4], argv[5]);
    }

    if(strcmp(argv[1],"-d") == 0) {
        decompress(argv[2], argv[3]);
    }

    return 0;
}

