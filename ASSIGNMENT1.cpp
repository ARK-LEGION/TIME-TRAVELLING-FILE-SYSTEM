//included standard libraries
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <fstream>
using namespace std;

//Implementing TreeNode class
class TreeNode{
private:
    int version_id;
    string content, message;
    time_t created_timestamp, snapshot_timestamp;
    TreeNode* parent;
    vector<TreeNode*> children;
public:
//defining constructor
    TreeNode(int version_id, string content, string message, time_t created_timestamp, time_t snapshot_timestamp, TreeNode* parent){
        this->version_id=version_id;
        this->content=content;
        this->message=message;
        this->created_timestamp=created_timestamp;
        this->snapshot_timestamp=snapshot_timestamp;
        this->parent=parent;
    }
    //required operations 
    int getVersionId(){
        return version_id;
    }
    string getContent(){
        return content;
    }
    void setContent(string new_content){
        content=new_content;
    }
    string getMessage(){
        return message;
    }
    void setMessage(string new_message){
        message=new_message;
    }
    time_t& getCreatedTimestamp(){
        return created_timestamp;
    }
    void setSnapshotTimestamp(time_t new_snapshot_time){
        snapshot_timestamp=new_snapshot_time;
    }
    time_t& getSnapshotTimestamp(){
        return snapshot_timestamp;
    }
    TreeNode* getParent(){
        return parent;
    }
    vector<TreeNode*> getChildren(){
        return children;
    }
    void addChild(TreeNode* child){
        children.push_back(child);
    }
    //destructor to free memory
    ~TreeNode(){
        for (TreeNode* child : children){
            delete child;
        }
    }
};

//CUSTOM PRIORITY QUEUE FOR BIGGER_TREES FUNCTION
class priority_queue_custom1 {
private:    
    vector<pair<int,string>> x;
    int parent(int i){
        return (i-1)/2;
    }
    int left(int i){
        return 2*i + 1;
    }
    int right(int i){
        return 2*i + 2;
    }

    void heapify(int i){
        int l=2*i + 1 ;
        int r=2*i + 2;
        int largest=i;
        if (l<(int)x.size() && x[largest]<x[l]) largest=l;
        if (r<(int)x.size() && x[largest]<x[r]) largest=r;
        if (largest!=i){
            swap(x[largest], x[i]);
            heapify(largest);
        }
    }
public: 

    bool isEmpty(){
        return x.empty();
    }
    int size(){
        return (int)x.size();
    }
    pair<int,string> top(){
        if(isEmpty()) return {-1,""};
        return  x[0];
    }
    void pop(){
        if (x.empty()) return;
        if (x.size()==1){ x.pop_back(); return;}
        x[0]=x[(int) x.size()-1];
        x.pop_back();
        if (!isEmpty()) heapify(0);
    }
    void push(pair<int,string> y){
        x.push_back(y);
        int cur= (int)x.size()-1;
        while (cur && x[parent(cur)]<x[cur]){
            swap(x[cur], x[parent(cur)]);
            cur=parent(cur);
        }
    }
};

// CUSTOM PRIORITY_QUEUE FOR RECENT_FILES FUNCTION
class priority_queue_custom2 {
private:    
    vector<pair<time_t,string>> x;
    int parent(int i){
        return (i-1)/2;
    }
    int left(int i){
        return 2*i + 1;
    }
    int right(int i){
        return 2*i + 2;
    }


    void heapify(int i){
        int l=2*i + 1 ;
        int r=2*i + 2;
        int largest=i;
        if (l<(int)x.size() && x[largest]< x[l]) largest=l;
        if (r<(int)x.size() && x[largest]< x[r]) largest=r;
        if (largest!=i){
            swap(x[largest], x[i]);
            heapify(largest);
        }
    }
public: 

    bool isEmpty(){
        return x.empty();
    }
    int size(){
        return (int)x.size();
    }
    pair<time_t,string> top(){
        if(isEmpty()) return {-1,""};
        return  x[0];
    }
    void pop(){
        if (x.empty()) return;
        if ((int)x.size()==1){ x.pop_back(); return;}
        x[0]=x[x.size()-1];
        x.pop_back();
        if (!isEmpty()) heapify(0);
    }
    void push(pair<time_t,string> y){
        x.push_back(y);
        int cur= (int)x.size()-1;
        while (cur && x[parent(cur)]<x[cur]){
            swap(x[cur], x[parent(cur)]);
            cur=parent(cur);
        }
    }
};

//implementing map form int to TreeNode* class (used in VersionControlSystem class)
class IntToNodeMap {
private:
    vector<vector<pair<int,TreeNode*>>> bucks;
    int cap;
    int elems;

    //integer hash function
    int h(int k){ if (k<0) k=-k; return k % cap; }

    void rehash(int newCap){
        vector<vector<pair<int, TreeNode*>>> old = bucks;
        bucks.assign(newCap, vector< pair<int, TreeNode*> >());
        cap = newCap;
        elems = 0;
        for (int i=0;i<(int)old.size();i++){
            for (int j=0;j<(int)old[i].size();j++){
                put(old[i][j].first, old[i][j].second);
            }
        }
    }

    //load factor to ensur that the hashmap doesn't get too crowded
    void load(){
        if (elems*4 >= cap*3){ // load factor ~0.75
            rehash(cap*2);
        }
    }
public:
    IntToNodeMap(){
        cap = 16;
        elems = 0;
        bucks.assign(cap, vector< pair<int, TreeNode*> >());
    }
    bool has(int key){
        int idx = h(key);
        for (int i=0;i<(int)bucks[idx].size();i++){
            if (bucks[idx][i].first==key) return true;
        }
        return false;
    }
    TreeNode* get(int key){
        int idx = h(key);
        for (int i=0;i<(int)bucks[idx].size();i++){
            if (bucks[idx][i].first==key) return bucks[idx][i].second;
        }
        return nullptr;
    }
    void put(int key, TreeNode* val){
        load();
        int idx = h(key);
        for (int i=0;i<(int)bucks[idx].size();i++){
            if (bucks[idx][i].first==key){
                bucks[idx][i].second = val;
                return;
            }
        }
        bucks[idx].push_back({key,val});
        elems++;
    }
};

//Implementing Version Control System class(File handling class)
class VersionControlSystem{
private:
    TreeNode* root;
    TreeNode* active_version;
    IntToNodeMap version_map;
    int total_versions;
    time_t last_modified_time;

public:
//constructor(CREATES FILE)
    VersionControlSystem(string initial_content){
        time_t now = time(0);
        last_modified_time=now;
        root = new TreeNode(0, initial_content, "Initial content", now, now, nullptr);
        active_version = root;
        version_map.put(0,root);
        total_versions = 1;
    }
    //destructor
    ~VersionControlSystem(){
        delete root;        
        root = nullptr;
        active_version = nullptr;

    }
    //required operations
    time_t get_last_modified_time(){
        return last_modified_time;
    }
    void touch(){
        last_modified_time = time(0);
    }
    int getVersionCount(){
        return total_versions;
    }
    void READ(){
        cout<<active_version->getContent()<<endl;
    }
    void INSERT(string content){
        time_t now=time(0);
        time_t prev_snapshot_time=active_version->getSnapshotTimestamp();
        if (prev_snapshot_time!=0){
            string new_content=active_version->getContent()+content;
            TreeNode* new_version=new TreeNode(total_versions, new_content, "", now, 0, active_version);
            active_version->addChild(new_version);
            active_version=new_version;
            version_map.put(total_versions,new_version);
            total_versions++;
        }
        else{
            string prev_content=active_version->getContent();
            active_version->setContent(prev_content+content);
        }
        touch();

    }
    void UPDATE(string content){
        if (active_version->getSnapshotTimestamp()!=0){
            time_t now=time(0);
            TreeNode* new_version=new TreeNode(total_versions, content, "", now, 0, active_version);
            active_version->addChild(new_version);
            active_version=new_version;
            version_map.put(total_versions,new_version);
            total_versions++;
        }
        else{
            active_version->setContent(content);
        }
        touch();
    }
    void SNAPSHOT(string message){
        if (active_version->getSnapshotTimestamp() != 0) {
            cout << "This version is already a snapshot!" << endl;
            return;
        }
        active_version->setSnapshotTimestamp(time(0));
        active_version->setMessage(message);
        touch();
    }
    void ROLLBACK(int versionID){
        if (version_map.has(versionID)){
            active_version=version_map.get(versionID);
        }
        else{
            cout<<"INVALID VERSION ID"<<endl;
        }
    }
    void ROLLBACK(){
        active_version=active_version->getParent();  
        if (active_version==nullptr){
            cout<<"NO PARENT VERSION EXISTS"<<endl;
            active_version=version_map.get(0);
        } 
        touch();
    }
    void HISTORY(){
        vector<TreeNode*> history;
        TreeNode* cur=active_version;
        while (cur){
            if (cur->getSnapshotTimestamp()!=0) history.push_back(cur);
            cur=cur->getParent();
        }
        if (history.empty()){
            cout<<"NO SNAPSHOTS TAKEN"<<endl;
        }
        //sorting to make chronological order from earliest modified version to the most recently mofified version
            //INCLUDES SNAPSHOTTED VERSIONS FROM ROOT TO CURRENT ACTIVE VERSION
        sort(history.begin(), history.end(), [&](TreeNode* a, TreeNode* b){
            return a->getSnapshotTimestamp()<b->getSnapshotTimestamp();
        });

        for (auto u:history){
            cout<<"Version ID: "<<u->getVersionId()<<", Message: "<<u->getMessage()<<", Snapshot taken at: "<<ctime(&u->getSnapshotTimestamp())<<endl;
        }

    }

};

//hashmap for string to VersionControlSystem* mapping (used in FileSystem class);
class StrToVcsMap {
private:
    vector<vector<pair<string,VersionControlSystem*>>> bucks;
    int cap;
    int elems;

    //polynomial rolling hash function to hash strings
    int hs(string s){
        long long h = 0;
        int B = 131;// taken a prime base
        for (int i=0;i<(int)s.size();i++){
            char c = (char)s[i];
            h = (h*B + c) % cap;
        }
        if (h<0) h += cap;
        return (int)h;
    }
    void rehash(int newCap){
        vector< vector< pair<string, VersionControlSystem*> > > old = bucks;
        bucks.assign(newCap, vector< pair<string, VersionControlSystem*> >());
        cap = newCap;
        elems = 0;
        for (int i=0;i<(int)old.size();i++){
            for (int j=0;j<(int)old[i].size();j++){
                put(old[i][j].first, old[i][j].second);
            }
        }
    }
    void load(){
        if (elems*4 >= cap*3){ // ~0.75 load function
            rehash(cap*2);
        }
    }
public:
    StrToVcsMap(){
        cap = 16;
        elems = 0;
        bucks.assign(cap, vector< pair<string, VersionControlSystem*> >());
    }
    bool has(string key){
        int idx = hs(key);
        for (int i=0;i<(int)bucks[idx].size();i++){
            if (bucks[idx][i].first==key) return true;
        }
        return false;
    }
    VersionControlSystem* get(string key){
        int idx = hs(key);
        for (int i=0;i<(int)bucks[idx].size();i++){
            if (bucks[idx][i].first==key) return bucks[idx][i].second;
        }
        return nullptr;
    }
    void put(string key, VersionControlSystem* val){
        load();
        int idx = hs(key);
        for (int i=0;i<(int)bucks[idx].size();i++){
            if (bucks[idx][i].first==key){
                bucks[idx][i].second = val;
                return;
            }
        }
        bucks[idx].push_back({key,val});
        elems++;
    }
    // For iteration (e.g., RECENT_FILES / BIGGEST_TREES)
    vector< pair<string, VersionControlSystem*> > items(){
        vector< pair<string, VersionControlSystem*> > v;
        for (int i=0;i<(int)bucks.size();i++){
            for (int j=0;j<(int)bucks[i].size();j++){
                v.push_back(bucks[i][j]);
            }
        }
        return v;
    }
};

//Implementing File System class
class FileSystem{
private:
    // map<string, VersionControlSystem*> files;
    StrToVcsMap files;
public: 
    ~FileSystem(){
        auto all = files.items();
        for (auto kv : all){
            delete kv.second;
        }
    }
    void CREATE(string filename){
        if (files.has(filename)){
            cout<<"FILE ALREADY EXISTS !"<<endl;
            return;
        }
        files.put(filename, new VersionControlSystem(""));
    }
    void READ(string filename){
        if (files.has(filename)==false){
            cout<<"FILE DOESN'T EXIST !"<<endl;
            return;
        }
        files.get(filename)->READ();
    }
    void INSERT(string filename, string content){
        if (files.has(filename)==false){
            cout<<"FILE DOESN'T EXIST !"<<endl;
            return;
        }
        files.get(filename)->INSERT(content);
    }
    void UPDATE(string filename, string content){
        if (files.has(filename)==false){
            cout<<"FILE DOESN'T EXIST !"<<endl;
            return;
        }
        files.get(filename)->UPDATE(content);
    }
    void SNAPSHOT(string filename, string message){
        if (files.has(filename)==false){
            cout<<"FILE DOESN'T EXIST !"<<endl;
            return;
        }
        files.get(filename)->SNAPSHOT(message);
    }
    void ROLLBACK(string filename, int versionID){
        if (files.has(filename)==false){
            cout<<"FILE DOESN'T EXIST !"<<endl;
            return;
        }
        files.get(filename)->ROLLBACK(versionID);
    }
    void ROLLBACK(string filename){
        if (files.has(filename)==false){
            cout<<"FILE DOESN'T EXIST !"<<endl;
            return;
        }
        files.get(filename)->ROLLBACK();
    }
    void HISTORY(string filename){
        if (files.has(filename)==false){
            cout<<"FILE DOESN'T EXIST !"<<endl;
            return;
        }
        files.get(filename)->HISTORY();
    }
    void RECENT_FILES(int num){
        priority_queue_custom2 pq;
        auto all=files.items();
        for (auto u:all){
            time_t t=u.second->get_last_modified_time();
            pq.push({t,u.first});
        }
        cout<<"THE FILES IN THE DESCENDING ORDER OF RECENT MODIFICATION ARE: "<<endl;
        while (!pq.isEmpty() && num>0){
            cout<<pq.top().second<<endl;
            num--;
            pq.pop();
        }
    }
    void BIGGEST_TREES(int num){
        priority_queue_custom1 pq;
        // priority_queue<pair<int,string>> pq;
        auto all=files.items();
        for (auto u:all){
            int sz=u.second->getVersionCount();
            pq.push({sz,u.first});
        }
        cout<<"THE FILES IN THE DESCENDING ORDER OF NUMBER OF VERSIONS ARE: "<<endl;
        while (!pq.isEmpty() && num>0){
            cout<<pq.top().second<<endl;
            num--;
            pq.pop();
        }
    }
};

//MAIN FUNCTION
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    FileSystem fs;

    auto trim = [](string& s){
        while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin());
        while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
    };

    string cmd;
    while (cin >> cmd) {
        if (cmd == "CREATE") {
            string filename; cin >> filename;
            fs.CREATE(filename);

        } else if (cmd == "READ") {
            string filename; cin >> filename;
            fs.READ(filename);

        } else if (cmd == "INSERT") {
            string filename; cin >> filename;
            string rest; getline(cin, rest); trim(rest);
            fs.INSERT(filename, rest);

        } else if (cmd == "UPDATE") {
            string filename; cin >> filename;
            string rest; getline(cin, rest); trim(rest);
            fs.UPDATE(filename, rest);

        } else if (cmd == "SNAPSHOT") {
            string filename; cin >> filename;
            string rest; getline(cin, rest); trim(rest);
            fs.SNAPSHOT(filename, rest);

        } else if (cmd == "ROLLBACK") {
            string filename; cin >> filename;
            string rest; getline(cin, rest);
            auto ltrim = [](string &s){ while(!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin()); };
            auto rtrim = [](string &s){ while(!s.empty() && isspace((unsigned char)s.back())) s.pop_back(); };
            ltrim(rest); rtrim(rest);

            if (rest.empty()) {
                fs.ROLLBACK(filename);
            } else if (all_of(rest.begin(), rest.end(),
                              [](char c){ return isdigit((unsigned char)c); })) {
                fs.ROLLBACK(filename, stoi(rest));
            } else {
                fs.ROLLBACK(filename);
            }

        } else if (cmd == "HISTORY") {
            string filename; cin >> filename;
            fs.HISTORY(filename);

        } else if (cmd == "RECENT_FILES") {
            int num; cin >> num;
            fs.RECENT_FILES(num);

        } else if (cmd == "BIGGEST_TREES") {
            int num; cin >> num;
            fs.BIGGEST_TREES(num);

        } else {// if the user typed an unknown command then discard the rest of the line
            string discard; getline(cin, discard);
            cerr << "Unknown command: " << cmd << "\n";
        }
    }
}



