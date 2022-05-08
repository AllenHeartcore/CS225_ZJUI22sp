#include <stdio.h>
#include <stdlib.h>
#define INF_NEG -2147483648
using namespace std;

class FibonacciNode {
public:
	int id;
	int priority;
	int degree;
	bool mark;
	FibonacciNode* parent;
	FibonacciNode* child;
	FibonacciNode* left;
	FibonacciNode* right;
	FibonacciNode(int id_input, int priority_input) {
		id = id_input;
		priority = priority_input;
		degree = 0;
		mark = false;
		left = this;
		right = this;
		parent = NULL;
		child = NULL;
	}
};

class FibonacciHeap {
public:
	int size;
	FibonacciNode* min;
	FibonacciHeap() {
		size = 0;
		min = NULL;
	}
	void insert_node(int id, int priority) {
		FibonacciNode* node = new FibonacciNode(id, priority);
		if (min == NULL) min = node;
		else {
			node->left = min->left;
			node->right = min;
			min->left->right = node;
			min->left = node;
			if (priority < min->priority) min = node;
		}
		size++;
	}
	FibonacciNode* minimum() {
		return min;
	}
	FibonacciNode* extract_min() {
		FibonacciNode* min_temp = min;
		if (min_temp == NULL) return NULL;
		else {
			if (min_temp->child != NULL) {
				FibonacciNode* iter = min_temp->child, *next_child;
				do {
					next_child = iter->right;
					iter->parent = NULL;
					iter->left = min->left;
					iter->right = min;
					min->left->right = iter;
					min->left = iter;
					iter = next_child;
				} while (iter != min_temp->child);
			}
			min_temp->left->right = min_temp->right;
			min_temp->right->left = min_temp->left;
			if (min_temp == min_temp->right) min = NULL;
			else {
				min = min_temp->right;
				consolidate();
			}
			size--;
			return min_temp;
		}
	}
	void consolidate() {
		int num_roots = 0;
		FibonacciNode* iter = min;
		do {
			iter = iter->right;
			num_roots++;
		} while (iter != min);
		FibonacciNode* prevs[size];
		for (int i = 0; i <= size; i++) prevs[i] = NULL;
		do {
			FibonacciNode* curr = iter, *next_node = curr->right;
			int deg = curr->degree;
			while (prevs[deg] != NULL) {
				FibonacciNode* prev = prevs[deg];
				if (curr->priority > prev->priority) {
					FibonacciNode* temp = curr;
					curr = prev;
					prev = temp;
				}
				link(prev, curr);
				prevs[deg] = NULL;
				deg++;
			}
			prevs[deg] = curr;
			iter = next_node;
		} while (--num_roots > 0);
		min = NULL;
		for (int i = 0; i <= size; i++) {
			if (prevs[i] != NULL) {
				if (min == NULL) min = prevs[i]->left = prevs[i]->right = prevs[i];
				else {
					prevs[i]->left = min->left;
					prevs[i]->right = min;
					min->left->right = prevs[i];
					min->left = prevs[i];
					if (prevs[i]->priority < min->priority) min = prevs[i];
				}
			}
		}
	}
	void link(FibonacciNode* new_child, FibonacciNode* new_parent) {
		new_child->left->right = new_child->right;
		new_child->right->left = new_child->left;
		new_child->parent = new_parent;
		if (new_parent->child != NULL) {
			new_child->left = new_parent->child->left;
			new_child->right = new_parent->child;
			new_parent->child->left->right = new_child;
			new_parent->child->left = new_child;
		} else {
			new_child->left = new_child;
			new_child->right = new_child;
			new_parent->child = new_child;
		}
		new_parent->degree++;
		new_child->mark = false;
	}
	int delete_node(int id) {
		if (decrease_key(id, INF_NEG) == 0) return 0;
		extract_min();
		return 1;
	}
	int decrease_key(int id, int k) {
		FibonacciNode* target = search_heap(id);
		if (target == NULL) {
			system("clear");
			printf("    >>> ERROR: Node with ID %d not found! <<<\n", id);
			return 0;
		}
		if (k > target->priority) {
			system("clear");
			printf("    >>> ERROR: New priority %d is larger than current priority %d! <<<\n", \
			k, target->priority);
			return 0;
		}
		target->priority = k;
		FibonacciNode* target_parent = target->parent;
		if (target_parent != NULL && target->priority < target_parent->priority) {
			cut(target, target_parent);
			cascading_cut(target_parent);
		}
		if (target->priority < min->priority) min = target;
		return 1;
	}
	void cut(FibonacciNode* target, FibonacciNode* target_parent) {
		target->left->right = target->right;
		target->right->left = target->left;
		target_parent->degree--;
		if (target_parent->degree == 0) target_parent->child = NULL;
		else if (target_parent->child == target) target_parent->child = target->right;
		target->left = min->left;
		target->right = min;
		min->left->right = target;
		min->left = target;
		target->parent = NULL;
		target->mark = false;
	}
	void cascading_cut(FibonacciNode* target_parent) {
		FibonacciNode* target_grandparent = target_parent->parent;
		if (target_grandparent != NULL) {
			if (target_parent->mark == false) target_parent->mark = true;
			else {
				cut(target_parent, target_grandparent);
				cascading_cut(target_grandparent);
			}
		}
	}
	int normalize_output(FibonacciNode* ptr) {
		if (ptr == NULL) return -1;
		else return ptr->priority;
	}
	void print_node(FibonacciNode* node) {
		printf("    [ID] %10d", node->id);
		printf("    [PRIORITY] %2d", node->priority);
		printf("    [DEGREE] %2d", node->degree);
		printf("    [MARK] %2d", node->mark);
		printf("    [PARENT] %3d", normalize_output(node->parent));
		printf("    [CHILD] %3d", normalize_output(node->child));
		printf("    [LEFT] %3d", normalize_output(node->left));
		printf("    [RIGHT] %3d\n", normalize_output(node->right));
	}
	void print_subheap(FibonacciNode* root) {
		if (root != NULL) {
			print_node(root);
			print_subheap(root->child);
			FibonacciNode* iter = root->right;
			while (iter != root) {
				print_node(iter);
				print_subheap(iter->child);
				iter = iter->right;
			}
		}
	}
	void print_heap() {
		printf("\n    <FIBONACCI HEAP with %d nodes>\n", size);
		print_subheap(min);
		printf("\n");
	}
	FibonacciNode* search_subheap(FibonacciNode* root, int id) {
		if (root != NULL) {
			if (root->id == id) return root;
			FibonacciNode* attempt = search_subheap(root->child, id);
			if (attempt != NULL) return attempt;
			FibonacciNode* iter = root->right;
			while (iter != root) {
				if (iter->id == id) return iter;
				attempt = search_subheap(iter->child, id);
				if (attempt != NULL) return attempt;
				iter = iter->right;
			}
		}
		return NULL;
	}
	FibonacciNode* search_heap(int id) {
		return search_subheap(min, id);
	}
};

int main() {
	int choice, id, priority;
	FibonacciHeap* h;
	h = new FibonacciHeap();
	system("clear");
	printf("    >>> Fibonacci Heap Simulator <<<\n\n");
	do {
		do {
			printf("    1. Insert a new node\n");
			printf("    2. Delete a node\n");
			printf("    3. Decrease priority\n");
			printf("    4. Extract min\n");
			printf("    5. Consolidate\n");
			printf("    0. Exit\n");
			printf("    Enter operation code: ");
		} while (scanf("%d", &choice) != 1 || choice < 0 || choice > 5);
		switch (choice) {
			case 1:
				printf("\n    [INSERT_NODE operation]\n");
				printf("    Enter node ID: ");
				scanf("%d", &id);
				if (h->search_heap(id) != NULL) {
					system("clear");
					printf("    >>> ERROR: Node with ID %d already exists! <<<\n", id);
					break;
				}
				printf("    Enter node priority: ");
				scanf("%d", &priority);
				h->insert_node(id, priority);
				system("clear");
				printf("    >>> Node %d inserted <<<\n", id);
				break;
			case 2:
				printf("\n    [DELETE_NODE operation]\n");
				printf("    Enter node ID: ");
				scanf("%d", &id);
				if (h->delete_node(id) == 1) {
					system("clear");
					printf("    >>> Node %d deleted <<<\n", id);
				}
				break;
			case 3:
				printf("\n    [DECREASE_KEY operation]\n");
				printf("    Enter node ID: ");
				scanf("%d", &id);
				if (h->search_heap(id) == NULL) {
					system("clear");
					printf("    >>> ERROR: Node with ID %d not found! <<<\n", id);
					break;
				}
				printf("    Enter new priority: ");
				scanf("%d", &priority);
				if (h->decrease_key(id, priority) == 1) {
					system("clear");
					printf("    >>> Priority of node %d decreased to %d <<<\n", id, priority);
				}
				break;
			case 4:
				printf("\n    [EXTRACT_MIN operation]\n");
				system("clear");
				if (h->size == 0) {
					printf("    >>> ERROR: Heap is empty! <<<\n");
					break;
				}
				printf("    >>> Minimum with priority %d extracted <<<\n", h->extract_min()->priority);
				break;
			case 5:
				printf("\n    [CONSOLIDATE operation]\n");
				system("clear");
				if (h->size == 0) {
					printf("    >>> ERROR: Heap is empty! <<<\n");
					break;
				}
				h->consolidate();
				printf("    >>> Heap consolidated <<<\n");
				break;
			case 0:
				system("clear");
				printf("    >>> Simulation terminated <<<\n\n");
				break;
			default:
				system("clear");
				printf("    >>> ERROR: Invalid operation code! <<<\n");
				break;
		}
		if (choice != 0) h->print_heap();
	} while (choice != 0);
	return 0;
}
