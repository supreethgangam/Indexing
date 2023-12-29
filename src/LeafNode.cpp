#include "RecordPtr.hpp"
#include "LeafNode.hpp"

LeafNode::LeafNode(const TreePtr &tree_ptr) : TreeNode(LEAF, tree_ptr)
{
    this->data_pointers.clear();
    this->next_leaf_ptr = NULL_PTR;
    this->prev_leaf_ptr = NULL_PTR;
    this->parent = NULL_PTR;
    if (!is_null(tree_ptr))
        this->load();
}

// returns max key within this leaf
Key LeafNode::max()
{
    auto it = this->data_pointers.rbegin();
    return it->first;
}

Key LeafNode::min(){
    auto it=this->data_pointers.begin();
    return it->first;
}

// inserts <key, record_ptr> to leaf. If overflow occurs, leaf is split
// split node is returned
// TODO: LeafNode::insert_key to be implemented
TreePtr LeafNode::insert_key(const Key &key, const RecordPtr &record_ptr)
{
    TreePtr new_leaf = NULL_PTR; // if leaf is split, new_leaf = ptr to new split node ptr

    if (this->is_full())
    {
        this->data_pointers[key] = record_ptr;
        this->size += 1;
        auto mid = this->data_pointers.begin();
        advance(mid, ceil(this->size / 2.0));

        auto newnode = new LeafNode();
        for (auto it = mid; it != this->data_pointers.end(); it++)
        {
            newnode->insert_key((*it).first, (*it).second);
            this->size -= 1;
        }
        auto nextptr=new LeafNode(this->next_leaf_ptr);
        nextptr->prev_leaf_ptr=newnode->tree_ptr;
        nextptr->dump();
        newnode->prev_leaf_ptr = this->tree_ptr;
        newnode->parent = this->parent;
        newnode->next_leaf_ptr=this->next_leaf_ptr;
        newnode->dump();
        this->next_leaf_ptr = (string)newnode->tree_ptr;
        this->data_pointers.erase(mid, this->data_pointers.end());
        this->dump();
        return newnode->tree_ptr;
    }
    else
    {
        this->data_pointers[key] = record_ptr;
        this->size += 1;
        this->dump();
    }
    //   cout << "LeafNode::insert_key not implemented" << endl;

    return new_leaf;
}

// key is deleted from leaf if exists
// TODO: LeafNode::delete_key to be implemented
void LeafNode::delete_key(const Key &key)
{
    TreePtr leftptr = this->prev_leaf_ptr;
    // cout << "lead node callllll" << endl;
    this->size -= 1;
    // cout<<is_null(this->data_pointers[key].block_ptr)<<"****"<<endl;
    if (is_null(this->data_pointers[key].block_ptr))
    {
        // cout<<"not found"<<endl;
        return;
        /* code */
    }
    
    this->data_pointers.erase(key);
    if (this->underflows() && !is_null(this->parent))
    {
        // cout<<"underflowed -------------"<<endl;

        if (!(is_null(leftptr)))
        {
            auto leftchild = new LeafNode(leftptr);
            if (leftchild->parent == this->parent)
            {
                if ((this->size) + leftchild->size >= 2 * ceil(FANOUT / 2.0) && leftchild->parent == this->parent)
                {
                    this->size += 1;
                    // cout << "redistribution" << endl;
                    auto start = leftchild->data_pointers.begin();
                    advance(start, leftchild->data_pointers.size() - 1);
                    this->data_pointers[(*start).first] = (*start).second;
                    leftchild->data_pointers.erase(start);
                    leftchild->size -= 1;
                    leftchild->dump();
                    this->dump();
                    return;
                }
                else
                {
                    auto curr = this->data_pointers;
                    for (auto it = this->data_pointers.begin(); it != this->data_pointers.end(); it++)
                    {
                        leftchild->data_pointers[(*it).first] = (*it).second;
                        leftchild->size += 1;
                    }
                    this->data_pointers.erase(this->data_pointers.begin(), this->data_pointers.end());
                    this->size = 0;
                    leftchild->next_leaf_ptr = this->next_leaf_ptr;
                    if (!is_null(this->next_leaf_ptr))
                    {
                        auto rightchild = new LeafNode(this->next_leaf_ptr);
                        rightchild->prev_leaf_ptr = leftchild->tree_ptr;
                        rightchild->dump();
                    }
                    leftchild->dump();
                    this->dump();
                    return;
                }
            }
        }

        // cout << "RIGHT CHILD CALLED" << endl;
        auto rightptr = this->next_leaf_ptr;
        auto rightchild = new LeafNode(rightptr);
        if (rightchild->parent == this->parent)
        {
            if ((this->size) + rightchild->size >= 2 * ceil(FANOUT / 2.0) && rightchild->parent == this->parent)
            {
                this->size += 1;
                // cout << "redistribution right " << endl;
                auto start = rightchild->data_pointers.begin();
                this->data_pointers[(*start).first] = (*start).second;
                rightchild->data_pointers.erase(start);
                rightchild->size -= 1;
                rightchild->dump();
                this->dump();
            }
            else
            {
                auto curr = this->data_pointers;
                for (auto it = this->data_pointers.begin(); it != this->data_pointers.end(); it++)
                {
                    rightchild->data_pointers[(*it).first] = (*it).second;
                    rightchild->size += 1;
                }
                this->data_pointers.erase(this->data_pointers.begin(), this->data_pointers.end());
                this->size = 0;
                rightchild->prev_leaf_ptr = this->prev_leaf_ptr;
                if (!is_null(this->prev_leaf_ptr))
                {
                    auto lfchild = new LeafNode(this->prev_leaf_ptr);
                    lfchild->next_leaf_ptr = rightchild->tree_ptr;
                    lfchild->dump();
                }
                rightchild->dump();
                this->dump();
            }
        }
    }

    // cout << "LeafNode::delete_key not implemented" << endl;
    this->dump();

}

// runs range query on leaf
void LeafNode::range(ostream &os, const Key &min_key, const Key &max_key) const
{
    BLOCK_ACCESSES++;

    // cout<<this->tree_ptr<<" "<<BLOCK_ACCESSES<<"***************"<<endl;
    for (const auto &data_pointer : this->data_pointers)
    {
        if (data_pointer.first >= min_key && data_pointer.first <= max_key)
            data_pointer.second.write_data(os);
        if (data_pointer.first > max_key)
            return;
    }
    auto curr=tree_node_factory(this->tree_ptr);
    if (!is_null(this->next_leaf_ptr) && max_key>curr->max())
    {
        auto next_leaf_node = new LeafNode(this->next_leaf_ptr);
        next_leaf_node->range(os, min_key, max_key);
        delete next_leaf_node;
    }
}

// exports node - used for grading
void LeafNode::export_node(ostream &os)
{
    TreeNode::export_node(os);
    for (const auto &data_pointer : this->data_pointers)
    {
        os << data_pointer.first << " ";
    }
    os << endl;
}

// writes leaf as a mermaid chart
void LeafNode::chart(ostream &os)
{
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    for (const auto &data_pointer : this->data_pointers)
    {
        chart_node += to_string(data_pointer.first) + " ";
    }
    chart_node += "]";
    os << chart_node << endl;
}

ostream &LeafNode::write(ostream &os) const
{
    TreeNode::write(os);
    for (const auto &data_pointer : this->data_pointers)
    {
        if (&os == &cout)
            os << "\n"
               << data_pointer.first << ": ";
        else
            os << "\n"
               << data_pointer.first << " ";
        os << data_pointer.second;
    }
    os << endl;
    os << this->next_leaf_ptr << endl;
    os << this->prev_leaf_ptr << endl;
    return os;
}

istream &LeafNode::read(istream &is)
{
    TreeNode::read(is);
    this->data_pointers.clear();
    for (int i = 0; i < this->size; i++)
    {
        Key key = DELETE_MARKER;
        RecordPtr record_ptr;
        if (&is == &cin)
            cout << "K: ";
        is >> key;
        if (&is == &cin)
            cout << "P: ";
        is >> record_ptr;
        this->data_pointers.insert(pair<Key, RecordPtr>(key, record_ptr));
    }
    is >> this->next_leaf_ptr;
    is >> this->prev_leaf_ptr;
    return is;
}