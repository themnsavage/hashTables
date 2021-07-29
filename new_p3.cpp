#include<iostream>
#include<string>
#include<fstream>
#include<cctype>
#include<vector>
#include<sstream>
#include<string.h>
using namespace std;
class Data{
    private:
    vector <string> *hash_table;
    vector < vector <int> > *line_pos;
    public:
    Data(int size){
        hash_table=new vector <string>(size);
        line_pos=new vector < vector <int> >(size);
    }
    bool is_empty(int index){
        if(hash_table->at(index).size()==0){
            return true;
        }
        return false;
    }
    bool is_same(int index,string word){
        if(!(is_empty(index))){
            if(word.compare(hash_table->at(index))==0){
                return true;
            }
        }
        return false;
    }
    void add_word(int index,string word){
        hash_table->at(index)=word;
    }
    void add_line(int word_index,int line_index){
        line_pos->at(word_index).push_back(line_index);
    }
    void print_line(int index,string word){
        cout<<word<<" appears on lines [";
        for(int i=0;i<line_pos->at(index).size();i++){
          if(i==(line_pos->at(index).size()-1)){
              cout<<line_pos->at(index).at(i);
          }
           else{
                cout<<line_pos->at(index).at(i)<<",";
           }
        }
        cout<<"]"<<endl;
    }
};
class Probing{
    private:
    int collisions;
    int unique_word;
    string probe_type;
    Data *data;
    int size;
    int half;
    public:
     Probing(int size,string type,int h){
        data=new Data(size);
        probe_type=type;
        collisions=0;
        unique_word=0;
        this->size=size;
        half=h;
    }
    int get_collisions(void){
        return collisions;
    }
    int get_unique(void){
        return unique_word;
    }
    unsigned long djb2(const char *word){
        unsigned long hash=5381;
        char c;
        while ((c = *word++)) {
            hash=((hash<<5)+hash)+c;
        }
        return hash;
    }
    int linear_probing(string word,int &coll,bool &same){
        int temp_col=0;
        unsigned long num=djb2(word.c_str())%this->size;
        bool found=false;
        int i=0;
        while(found==false){
             int index=((num+i)%this->size);
             if(data->is_empty(index)){
                return index;
             }
             else{
                 if(data->is_same(index,word)){
                     same=true;
                   return index;
                 }
                else{
                   coll++;
                }
             }
             i++;
        }
        return -1;
    }
    int quadratic_probing(string word,int &col,bool &same){
        int temp_col=0;
        unsigned long num=djb2(word.c_str())%this->size;
        for(int i=0;i<this->size;i++){
            int index=(num+i*i)%this->size;
            if(data->is_empty(index)){
                return index;
            }
            else if(data->is_same(index,word)){
                same=true;
                return index;
            }
            else{
                col++;
            }
        }
        return -1;
    }
    int double_hashing(string word,int &col,bool &same){
        unsigned long num=djb2(word.c_str());
        for(int i=0;i<this->size;i++){
            int index=((num % this->size)+ i * (half-(num % half)))%this->size;
            if(data->is_empty(index)){
                return index;
            }
            else if(data->is_same(index,word)){
                same=true;
                return index;
            }
            else{
                col++;
            }
        }
        return -1;
    }

    void probe_word(string word, int line_index){
        bool same=false;
        int index;
        if(probe_type.compare("lp")==0){
            index=linear_probing(word,collisions,same);
        }
        else if(probe_type.compare("qp")==0){
            index=quadratic_probing(word,collisions,same);
        }
        else if(probe_type.compare("dh")==0){
           index=double_hashing(word,collisions,same);
        }
        
        // else{
        //     cout<<"--store word function error: store type does not match--"<<endl;
        // }
        if(index>0 && same==false){
            data->add_word(index,word);
            data->add_line(index,line_index);
            unique_word++;
        }
        else if (index>0 && same == true){
            data->add_line(index,line_index);
        }
        else{
            unique_word++;
        }
    }
    void search_word(string word){
        int temp_coll=0;
        int index;
        bool same=false;
        if(probe_type.compare("lp")==0){
            index=linear_probing(word,temp_coll,same);
        }
        else if(probe_type.compare("qp")==0){
            index=quadratic_probing(word,temp_coll,same);
        }
         else if(probe_type.compare("dh")==0){
             index=double_hashing(word,temp_coll,same);
             }
        // else{
        //     cout<<"--store word function error: store type does not match--"<<endl;
        // }
            data->print_line(index,word);
            cout<<"The search had "<<temp_coll<<" collisions"<<endl;
    }
};
int main(int argc, char *argv[]){
    ifstream input_file,query_file;
    int hash_size=stoi(argv[3]);
    string store_type=argv[4];
    int half=0;
    if(store_type.compare("dh")==0){
        half=stoi(argv[5]);
    }
    Probing prob=Probing(hash_size,store_type,half);
    string line;
    char letter;
    string word;
    int num_word=0;
    int line_num=1;
    input_file.open(argv[1]);
    while(getline(input_file,line)){
        stringstream iss(line);
        while(iss.get(letter)){
            if(isalpha(letter)){
                word+=tolower(letter);
            }
            else{
                if(word.size()>0){
                    num_word++;
                    prob.probe_word(word,line_num);
                    word="";
                }
            }
        }
        if(word.size()>0){
            num_word++;
            prob.probe_word(word,line_num);
        }
        word="";
        line_num++;
    }
    input_file.close();
    cout<<"The number of words found in the file was "<<num_word<<endl;
    cout<<"The number of unique words found in the file was "<<prob.get_unique()<<endl;
    cout<<"The number of collisions was "<<prob.get_collisions()<<endl;
    query_file.open(argv[2]);
    string look_word="";
    int first=1;
    while(query_file>>look_word){
        if(first==1){
            cout<<endl;
        }
        prob.search_word(look_word);
    }
}