//
// Created by Hila Cohen on 16/11/2022.
//


#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "ProductBST.h"



/**check if line is valid-after : there are digits
 * in case of negative quantity I referred it somewhere else*/
bool is_valid_line(char* line){
  for (int i = 0; i < (int)(strlen(line)) ; ++i)
  {
    if(line[i]==':'){
      int j=i+2;
      while(line[j]!='\n' && line[j]!='\r'){
          if(!isdigit(line[j])){
            return false;
          }
          j+=1;
      }
    }

  }
  return true;
}

/** allocate and assign new name for node given*/
char* allooc_name(Node* node, char* name){
  size_t len_str = strlen(name);
  node->product.name= malloc ((strlen (name)+1)*sizeof (char));
  if(!node->product.name){
    fprintf(stderr,ERROR);
    fprintf (stderr,ALLOCATION_FAILED);
    return NULL;
  }
  size_t i;
  for(i=0;i<len_str;i++)
  {
    node->product.name[i]=name[i];
  }
  node->product.name[i] = '\0';
  return node->product.name;
}

/**return allocated node in tree in case of node(root) is null return null*/
Node* helper_add_product(Node *node, char *name, int quantity){
  ///if root is NULL create new root
  ///root is null
  if(!node){
    Node* new_root = malloc(sizeof(Node));
    if(!new_root){
      fprintf(stderr,ERROR);
      fprintf (stderr,ALLOCATION_FAILED);
      return NULL;
    }
    new_root->product.name= allooc_name (new_root,name);
    if(!new_root->product.name){
      free (new_root);
      return NULL;
    }
    new_root->product.quantity=quantity;
    return new_root;


  }

  ///if root is not null
  Node* right_child= node->right_child;
  Node* left_child=node->left_child;
  int lexicography_order=strcmp(node->product.name,name);

  ///if new name should  be left sub tree of node and node is leaf
  ///add a new node as left child of node
  if (lexicography_order>0 && !left_child){
    node->left_child= malloc(sizeof(Node));
    if(!node->left_child){
        fprintf(stderr,ERROR);
        fprintf (stderr,ALLOCATION_FAILED);
        return NULL;
    }
    node->left_child->product.name= allooc_name (node->left_child,name);
    if(!node->left_child->product.name){
      free (node->left_child);
      return NULL;
    }
    node->left_child->product.quantity=quantity;
    return node->left_child;

  }
  else if (lexicography_order<0 && !right_child){
    node->right_child= malloc(sizeof(Node));
    if(!node->right_child){
      fprintf(stderr,ERROR);
      fprintf (stderr,ALLOCATION_FAILED);
      return NULL;
    }
    node->right_child->product.name= allooc_name (node->right_child,name);
    if(!node->right_child->product.name){
      free (node->right_child);
      return NULL;
    }
    node->right_child->product.quantity=quantity;
    return node->right_child;
  }
  ///we should continue searching down the tree
  else{
     ///should be on right sun tree
     if(lexicography_order<0){
       return helper_add_product (node->right_child,name,quantity);
     }
     ///should be on left sub tree
     else{
       return helper_add_product (node->left_child,name,quantity);
     }
  }

}

/**
 * This function adds product to the BST with a lexicographic order of the
 * Products names.
 * It is the responsibility of this function to allocate memory for new node
 * and for name's string.
 * You cannot assume that the name isn't already in the tree.
 * You cannot assume that the quantity is positive.
 * Hint - Consider using a recursive helper function.
 * @param root : pointer to the root node of the BST.
 * @param name : string containing product's name
 * @param quantity : integer representing the existing quantity from
 * this product.
 * @return pointer to the root node of BST after adding a new product.
 * In case of memory allocation failure - return NULL.
 * In case of invalid quantity (zero or negative) - print error massage to
 * stderr and return the original tree, means - without adding the product.
 */

Node *add_product (Node *root, char *name, int quantity){
  bool root_is_null=(root==NULL);
  ///error-this name already exists in tree
  if(search_product (root,name)!=NULL){
    fprintf(stderr,ERROR);
    fprintf (stderr,PRODUCT_EXISTS);
    return root;
  }
  ///quantity is NOT POSITIVE
  if(quantity<=0){
    fprintf(stderr,ERROR);
    fprintf (stderr,INVALID_QUANTITY);
    return root;

  }

  ///else-find the product in tree
  Node* node_inserted= helper_add_product (root,name,quantity);
  if(!node_inserted){
    return NULL;
  }
  ///if root is NULL insert and return new root
  if(root_is_null){
    root=node_inserted;
  }
  node_inserted->left_child=NULL;
  node_inserted->right_child=NULL;
  return root;

}




/**
 * This function reads line after line from the file and
 * builds binary search tree. Each node at the BST represents different
 * product and contains its quantity. The order relation between the products
 * is according to a lexicographic order of the products names.
 * Hint - consider using the function add_product as a helper function.
 * @param filename : name of text file that contains names of products and
 * theirs quantities.
 * @return pointer to the root node of the BST, or null in case of memory
 * allocation failure.
 */
Node *build_bst (const char *filename){
  Node* root=NULL;
  ///get data from file
  FILE    *textfile;
  char    line[MAX_LINE_LENGTH];
  textfile = fopen(filename, "r");
  ///error in text file
  if(textfile == NULL){
    fprintf(stderr,ERROR);
    fprintf (stderr,FILE_OPEN_FAILED);
    return NULL;
  }

  ///get line by line
  while(fgets(line, MAX_LINE_LENGTH, textfile)){
    int quantity;
    bool valid_line=is_valid_line(line);

    char name[MAX_LINE_LENGTH];
    ///assign name and quantity
    int scanf_result= sscanf (line,"%[^:]:%d",name,&quantity);

    ///quantity is NOT POSITIVE
    if(quantity<0){
      fprintf(stderr,ERROR);
      fprintf (stderr,INVALID_QUANTITY);

    }
    if(!valid_line && quantity>=0){
      fprintf(stderr,ERROR);
      fprintf (stderr,INVALID_LINE);
    }

    if(scanf_result!=EOF && valid_line){
    ///create new node
      root= add_product (root,name,quantity);
    }
  }
  fclose(textfile);
  return root;

}



/**return product pointer of given name else, case of error
 * product not found or root is null-return null */
Product* helper_search_product(Node *node, char *name){
    Product* p;
    if(!node){
      p=NULL;
      return NULL;
    }
    int cmp_str_name= strcmp(node->product.name,name);
    if(cmp_str_name==0){
      return &(node->product);
    }

    if(cmp_str_name<0){
      if(!node->right_child){
        return NULL;
      }
      else{
        p=helper_search_product(node->right_child,name);
      }
    }
    if(cmp_str_name>0){
      if(!node->left_child){
        return NULL;
      }
      else{
        p= helper_search_product(node->left_child,name);
      }
    }
    return p;

}


/**
 * Search product at BST by product's name.
 * @param root : pointer to root node of BST.
 * @param name : string with name to be searched.
 * @return pointer to the found product. In case the product does not exist,
 * or in case that the input root in NULL, return null.
 */
Product *search_product (Node *root, char *name){
  //root is null
  if(!root ){
    return NULL;
  }
  if(!name ){
    fprintf(stderr,ERROR);
    fprintf (stderr,INVALID_POINTER);
    return NULL;
  }
  Product* prod_found=helper_search_product(root,name);
  //product does not exists
  if(!prod_found){
    return NULL;
  }
  return prod_found;



}

/**return true if given child shuold be in right sub tree
 *of father else-return false*/
bool is_child_in_right_subtree(Node* child,Node* father){
    if(strcmp (child->product.name,father->product.name)>0){
      return true;
    }
  return false;
}



/***/
Node* helper_delete_node(Node* node, char* name_to_del){
      if(!node){
        return NULL;
      }
      ///node should  be deleted
      if(strcmp(node->product.name,name_to_del)==0){
        return node;
      }
      ///right child should be deleted
      char* name_of_right_chld=node->right_child->product.name;
      if(node->right_child && strcmp (name_to_del,name_of_right_chld)>0){
        helper_delete_node (node->right_child,name_to_del);
      }
      ///left child should be deleted
      else if(node->right_child && strcmp (name_to_del,name_of_right_chld)<0 ){
        helper_delete_node (node->left_child,name_to_del);

      }
      return NULL;
}
/**return sucssesor of the right tree
 * if there are no left child return given node*/
Node* find_successor(Node* node){
  Node* curr_succ=node;
  while(curr_succ && curr_succ->left_child){
    curr_succ=curr_succ->left_child;

  }
  return curr_succ;
}

/**
 * Given a binary search tree (root) and a name of product, this function
 * deletes the node with this name and returns root of the updated tree.
 * In case the name does not exists at the tree, the function will return the
 * root as is, and will print error massage to stderr.
 * Hint - consider the three cases of the node to be deleted:
 * 1) Node is a leaf (has no children): Simply remove the leaf from the tree.
 * 2) Node has only one child: The node of the child of the node to be deleted
 * is now father's child (more can be found at the PDF).
 * 3) Node has two children: Find inorder successor of the node. Copy
 * contents of the inorder successor to the node and delete the inorder
 * successor.
 * (Don't forget freeing old memory and allocating a new one validly.)
 * @param root : pointer to root node
 * @param name : string with name of product to be deleted
 * @return pointer to the updated BST root after deleting.
 * In case that input root is null, the function will return null.
 */
Node *delete_product (Node *root, char *name){
  ///name does not exists at the tree

  if(!root ){
    fprintf(stderr,ERROR);
    fprintf (stderr,PRODUCT_NOT_FOUND);
    return root;
  }
  if(!name){
    fprintf(stderr,ERROR);
    fprintf (stderr,INVALID_POINTER);
    return root;
  }
  else if(strcmp (name,root->product.name)<0){
     root->left_child= delete_product (root->left_child,name);
  }
  else if(strcmp (name,root->product.name)>0){
    root->right_child= delete_product (root->right_child,name);
  }
  ///this is the root to be deleted
  else{

    ///only right child or non
    if(!root->left_child){
      Node* tmp=root->right_child;
      //root=root->right_child;
      free (root->product.name);
      free(root);
      return tmp;

    }
    ///only left child
    else if(!root->right_child){
      Node* tmp=root->left_child;
      //root=root->left_child;
      free (root->product.name);
      free(root);
      return tmp;
    }
    ///2 children
    Node* tmp=find_successor(root->right_child);
    free (root->product.name);
    root->product.name= allooc_name (root,tmp->product.name);

    if(!root->product.name){
      fprintf(stderr,ERROR);
      fprintf (stderr,ALLOCATION_FAILED);
      return root;
    }
    root->product.quantity=tmp->product.quantity;
    root->right_child= delete_product (root->right_child, tmp->product.name);

  }

  ///root of the updated tree
  return root;
}


/**
 * This function frees BST's allocated memory.
 * @param root : pointer to root node of BST.
 */
void delete_tree (Node *root){
    if(!root){
      return;
    }
    delete_tree(root->left_child);
    delete_tree(root->right_child);
    free(root->product.name);
    free(root);
    root=NULL;
}


/**
 * Update quantity of a product in the BST. You cannot assume the name is
 * already exists in the tree.
 * @param root : pointer to the root node of tree.
 * @param name : string with name of product to be updated.
 * @param amount_to_update : integer with amount to be updated. You should
 * add this value to the quantity of the product. If this value is negative,
 * You should subtract the absolute value from the existing quantity.
 * @return root of tree after performing the update.
 * In case of failure (name is not found, root is NULL or the **updated**
 * quantity is negative) - print error massage and do nothing (return
 * the original root).
 * In case that the **updated** quantity is exactly zero - return the root
 * of tree after deleting this product.
 */
Node* update_quantity (Node *root, char *name, int amount_to_update){
  Product* prod=search_product (root,name);
  ///name is null(msg already printed to stderr)
  if(!name){
    return root;
  }
  ///product not in tree
  if(!prod){
    fprintf(stderr,ERROR);
    fprintf (stderr,PRODUCT_NOT_FOUND);
    return root;
  }

  ///root is NULL
  if(!root){
    fprintf(stderr,ERROR);
    fprintf (stderr,INVALID_POINTER);
    return root;
  }

  ///updated quantity is negative
  if(prod->quantity+amount_to_update<0 ){
    fprintf(stderr,ERROR);
    fprintf (stderr,INVALID_QUANTITY);
    return root;
  }
  ///updated quantity is 0- remove product
  if(prod->quantity+amount_to_update==0 ){
    delete_product (root,name);
    return root;
  }
  ///update quantity
  prod->quantity+=amount_to_update;
  return root;

}

