/*                                                                  */
/*   POLYLIST.H  :  Header file for polynomial library              */
/*                                                                  */
/*                     Programmed By Kim Junmo                      */
/*                       Modified By Lee Jaekyu                     */

#ifndef _POLYLIST_H
#define _POLYLIST_H

typedef struct _node
    {
    int degree;
    double coef;
    struct _node *next;
    } node;

node *make_node(int degree, double coef);
void insert_node(node *head, int degree, double coef);
double pow_simple(double x, int order);
void remove_polynomial(node *a);
node *lagrange(double points[][2], int n);

#endif




