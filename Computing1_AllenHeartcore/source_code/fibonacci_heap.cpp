#include <stdio.h>
#include "medical_system.h"
#include <stdint.h>
using namespace std;

centernode::centernode(personalinfo* patient) {
	id = patient->id;
	priority = patient->priority;
	person = patient;
	degree = 0;
	mark = false;
	left = this;
	right = this;
	parent = NULL;
	child = NULL;
}
centerqueue::centerqueue() {
	size = 0;
	min = NULL;
}
int centerqueue::insert_node(centernode* node) {
	if (search_heap(node->id) != NULL) return -1;   // ERROR: Existing ID
	if (min == NULL) min = node;
	else {
		node->left = min->left;
		node->right = min;
		min->left->right = node;
		min->left = node;
		if (node->priority < min->priority) min = node;
	}
	size++;
	return 0;
}
int centerqueue::delete_node(int id) {
	if (decrease_key(id, INF_NEG) == -1) return -1;   // ERROR: Non-existing ID
	centernode* node = extract_min();
	blacklist.push_back(node->person);
	return 0;
}
int centerqueue::decrease_key(int id, uint64_t priority) {   // Better be called "modify_key"
	centernode* target = search_heap(id);
	if (target == NULL) return -1;   // ERROR: Non-existing ID
	if (priority == target->priority) return 0;   // Priority unchanged
	else {
		target->priority = priority;
		if (priority < target->priority) {   // Priority decreased
			centernode* target_parent = target->parent;
			if (target_parent != NULL && target->priority < target_parent->priority) {
				cut(target, target_parent);   // Reorganize heap
				cascading_cut(target_parent);
			}
			if (target->priority < min->priority) min = target;
		} else {   // Priority increased
			if (target->child != NULL) {
				centernode* iter = target->child, *next_child;
				do {
					next_child = iter->right;
					iter->parent = NULL;   // "Float up" all children
					iter->left = min->left;
					iter->right = min;
					min->left->right = iter;
					min->left = iter;
					iter = next_child;
				} while (iter != target->child);
			}
		}
	}
	return 0;
}
void centerqueue::cut(centernode* target, centernode* target_parent) {
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
void centerqueue::cascading_cut(centernode* target_parent) {
	centernode* target_grandparent = target_parent->parent;
	if (target_grandparent != NULL) {
		if (target_parent->mark == false) target_parent->mark = true;
		else {
			cut(target_parent, target_grandparent);
			cascading_cut(target_grandparent);
		}
	}
}
centernode* centerqueue::extract_min() {
	centernode* min_temp = min;
	if (min_temp == NULL) return NULL;   // ERROR: Empty heap
	else {
		if (min_temp->child != NULL) {
			centernode* iter = min_temp->child, *next_child;
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
int centerqueue::consolidate() {
	if (min == NULL) return -1;   // ERROR: Empty heap
	int num_roots = 0;
	centernode* iter = min;
	do {
		iter = iter->right;
		num_roots++;
	} while (iter != min);   // Count roots
	centernode* prevs[size];
	for (int i = 0; i <= size; i++) prevs[i] = NULL;
	do {
		centernode* curr = iter, *next_node = curr->right;
		int deg = curr->degree;
		while (prevs[deg] != NULL) {   // Link required
			centernode* prev = prevs[deg];
			if (curr->priority > prev->priority) {
				centernode* temp = curr;
				curr = prev;
				prev = temp;
			}
			link(prev, curr);
			prevs[deg] = NULL;
			deg++;
		}
		prevs[deg] = curr;   // Update degree pointers
		iter = next_node;
	} while (--num_roots > 0);
	min = NULL;
	for (int i = 0; i <= size; i++) {   // Link trees into a heap
		if (prevs[i] != NULL) {
			if (min == NULL) min = prevs[i]->left = prevs[i]->right = prevs[i];
			else {
				prevs[i]->left = min->left;
				prevs[i]->right = min;
				min->left->right = prevs[i];
				min->left = prevs[i];
				if (prevs[i]->priority < min->priority) min = prevs[i];
			}   // Determine the new "min" meanwhile
		}
	}
	return 0;
}
void centerqueue::link(centernode* new_child, centernode* new_parent) {
	new_child->left->right = new_child->right;   // Delete from root list
	new_child->right->left = new_child->left;
	new_child->parent = new_parent;   // Connect to new parent
	if (new_parent->child != NULL) {
		new_child->left = new_parent->child->left;   // Insert into child list
		new_child->right = new_parent->child;
		new_parent->child->left->right = new_child;
		new_parent->child->left = new_child;
	} else {
		new_child->left = new_child;   // Create a new child list
		new_child->right = new_child;
		new_parent->child = new_child;
	}
	new_parent->degree++;
	new_child->mark = false;
}
centernode* centerqueue::search_subheap(centernode* root, int id) {
	if (root != NULL) {
		if (root->id == id) return root;   // Self
		centernode* attempt = search_subheap(root->child, id);
		if (attempt != NULL) return attempt;   // Children
		centernode* iter = root->right;
		while (iter != root) {
			if (iter->id == id) return iter;   // Siblings
			attempt = search_subheap(iter->child, id);
			if (attempt != NULL) return attempt;   // Siblings' children
			iter = iter->right;
		}
	}
	return NULL;
}
centernode* centerqueue::search_heap(int id) {
	return search_subheap(min, id);
}
