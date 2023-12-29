#include "InternalNode.hpp"
#include<bits/stdc++.h>
//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    this->parent=NULL_PTR;
    if (!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

Key InternalNode::min(){
    Key min_key = DELETE_MARKER;
    TreeNode* first_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    min_key = first_tree_node->min();
    delete first_tree_node;
    return min_key;


}
//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_tree_ptr = NULL_PTR;
    vector<Key> keys=this->keys;
    int ind;
    auto index=lower_bound(keys.begin(),keys.end(),key);
    ind=index-keys.begin();
    TreePtr insertnode=this->tree_pointers[ind];
    TreeNode * node=tree_node_factory(insertnode);
    TreePtr split= node->insert_key(key,record_ptr);
    if(!is_null(split)){
        auto leftnode=tree_node_factory(insertnode);
        this->keys.push_back(leftnode->max());
        sort(this->keys.begin(),this->keys.end());
        int curr=lower_bound(this->keys.begin(),this->keys.end(),leftnode->max())-this->keys.begin()+1;
        this->tree_pointers.insert(this->tree_pointers.begin()+curr,split);
        this->size+=1;
        if(this->overflows()){
            auto mid=this->keys.begin();
            // cout<<this->keys.size()<<endl;
            advance(mid,ceil(this->keys.size()/2.0));
            auto newnode=new InternalNode();
            for(auto it=mid;it!=this->keys.end();it++){
                newnode->keys.push_back(*it);
            }
            auto midpointer=this->tree_pointers.begin();
            advance(midpointer,ceil(this->tree_pointers.size()/2.0));
            for(auto it=midpointer;it!=this->tree_pointers.end();it++){
                auto child_ptrs=tree_node_factory(*it);
                child_ptrs->parent=newnode->tree_ptr;
                child_ptrs->dump();
                newnode->tree_pointers.push_back(*it);
                newnode->size+=1;
            }
            newnode->dump();
        
            this->keys.erase(mid-1,this->keys.end());
            this->size=this->keys.size()+1;
            this->tree_pointers.erase(midpointer-1,this->tree_pointers.end());
            this->dump();
            return newnode->tree_ptr;
        }
    }
    this->dump();
    return new_tree_ptr;
}

//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr new_tree_ptr = NULL_PTR;
    vector<Key> keys=this->keys;
    int ind;
    auto index=lower_bound(keys.begin(),keys.end(),key);
    ind=index-keys.begin();
    auto deletenode=this->tree_pointers[ind];
    auto node =tree_node_factory(deletenode);
    node->delete_key(key);
    vector<int> temp;
    int point=-1;
    int dec=0;
    for(int i=0;i<this->size;i++){
        auto ptr=this->tree_pointers[i];
        auto childnode=tree_node_factory(ptr);
        if(childnode->size==0){
            point =i;
            // cout<<childnode->tree_ptr<<" >>>>>>>> " <<this->tree_ptr<<endl;
            dec+=1;
        }
        else{
            temp.push_back(childnode->max());
        }
    }
    this->size-=dec;
    if(point!=-1){
        this->tree_pointers.erase(tree_pointers.begin()+point);
    }
    this->keys=temp;

    if(this->underflows()){
        auto parentptr=this->parent;
        // cout<<"underflow again *****************"<<parentptr<<endl;
        if(!is_null(parentptr)){
            auto parentnode=new InternalNode(parentptr);
            auto parent_ptr=parentnode->tree_pointers;
            auto it=find(parent_ptr.begin(),parent_ptr.end(),this->tree_ptr);
            if(it!=parent_ptr.begin()){
              
                advance(it,-1);
                auto leftsibling=new InternalNode(*it);
                if(leftsibling->size+this->size >= 2 * ceil(FANOUT / 2.0)){
                    cout<<"case 1 left redistribution "<<endl;
                    cout << "redistribution" << endl;
                    auto start = leftsibling->tree_pointers.begin();
                    this->keys[0]=leftsibling->max();
                    advance(start, leftsibling->tree_pointers.size() - 1);
                    auto addnode= tree_node_factory(*start);
                    addnode->parent=this->tree_ptr;
                    addnode->dump();
                    // cout<<addnode->tree_ptr<<"  ============== "<<addnode->parent<<endl;
                    this->tree_pointers.insert(this->tree_pointers.begin(),*start);
                    leftsibling->tree_pointers.erase(start);
                    leftsibling->size -= 1;
                    this->size+=1;
                    leftsibling->dump();
                    this->dump();
                    return;

                }
                else{
                    if(leftsibling->size+this->size<=FANOUT){
                        // cout<<"case 2 left merging "<<endl;
                        while(this->size>0){
                            leftsibling->keys.push_back(leftsibling->max());
                            leftsibling->tree_pointers.push_back(this->tree_pointers[0]);
                            leftsibling->size+=1;
                            auto cpr=tree_node_factory(this->tree_pointers[0]);
                            cpr->parent=leftsibling->tree_ptr;
                            cpr->dump();
                            auto beg=this->tree_pointers.begin();
                            this->tree_pointers.erase(beg);
                            auto keybeg=this->keys.begin();
                            // this->keys.erase(this->keys.begin());
                            this->size-=1;
                            this->dump();
                            leftsibling->dump();
                            // cout<<"ENDING ----------------- AGAIN>"<<endl;
                        }
                        return;
                    }
                }
            }
            it=find(parent_ptr.begin(),parent_ptr.end(),this->tree_ptr);
            if(it!=parent_ptr.begin()+parent_ptr.size()-1){
                 advance(it,1);

                auto rightsibling=new InternalNode(*it);
                if(rightsibling->size+this->size >= 2 * ceil(FANOUT / 2.0)){
                    // cout << "3 right redistribution" << endl;
                    auto start = rightsibling->tree_pointers.begin();
                    auto addnode= tree_node_factory(*start);
                    addnode->parent=this->tree_ptr;
                    addnode->dump();
                    // cout<<addnode->tree_ptr<<"  ============== "<<addnode->parent<<endl;
                    this->keys.push_back(this->max());
                    this->tree_pointers.push_back(*start);
                    rightsibling->keys.erase(rightsibling->keys.begin());
                    rightsibling->tree_pointers.erase(start);
                    rightsibling->size -= 1;
                    rightsibling->dump();
                    this->size+=1;
                  
                    this->dump();
                    return;

                }
                else{
                    if(rightsibling->size+this->size<=FANOUT){
                        //  cout << "4 right merging" << endl;
                        while(this->size>0){
                            rightsibling->tree_pointers.insert(rightsibling->tree_pointers.begin(),this->tree_pointers[this->tree_pointers.size()-1]);
                            rightsibling->size+=1;
                            // cout <<this->size<<"*************"<<endl;
                            auto cpr=tree_node_factory(this->tree_pointers[this->tree_pointers.size()-1]);
                            cpr->parent=rightsibling->tree_ptr;
                            cpr->dump();
                            auto beg=this->tree_pointers.begin()+this->tree_pointers.size()-1;
                            this->tree_pointers.erase(beg);
                            auto keybeg=this->keys.begin();
                            auto key_beg=rightsibling->keys.begin();
                            rightsibling->keys.insert(key_beg,this->max());
                            // cout <<this->size<<"*************"<<endl;
                            this->size-=1;
                            this->dump();
                            rightsibling->dump();
                            // cout <<this->size<<"*************"<<endl;
                        }
                        return;

                    }

                }


            }
            
        }
        // cout<<"ending now"<<endl;
    }
    this->dump();
    // this->dump();
}



//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    // cout<<this->tree_ptr<<" ***********  "<<BLOCK_ACCESSES<<endl;
    for (int i = 0; i < this->size - 1; i++) {
        if (min_key <= this->keys[i]) {
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++) {
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i - 1]) + " < x";
        } else {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    // cout<<"****************"<<endl;
        if(this->size>0){
        for (int i = 0; i < this->size - 1; i++) {
            if (&os == &cout)
                os << "\nP" << i + 1 << ": ";
            os << this->tree_pointers[i] << " ";
            if (&os == &cout)
                os << "\nK" << i + 1 << ": ";
            os << this->keys[i] << " ";
        }
        if (&os == &cout)
            os << "\nP" << this->size << ": ";
        os << this->tree_pointers[this->size - 1];
    }
    // cout<<"****************"<<endl;
    return os;
}

istream& InternalNode::read(istream& is) {
    TreeNode::read(is);
    if(this->size>0){
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
   
        for (int i = 0; i < this->size - 1; i++) {
            if (&is == &cin)
                cout << "P" << i + 1 << ": ";
            is >> this->tree_pointers[i];
            if (&is == &cin)
                cout << "K" << i + 1 << ": ";
            is >> this->keys[i];
        }
        if (&is == &cin)
            cout << "P" << this->size;
        is >> this->tree_pointers[this->size - 1];
    }
    return is;
}
