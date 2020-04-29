
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <unordered_map>
#include <string>
#include <time.h>
#include <algorithm>
#include <fstream>
using namespace std;
typedef unsigned int DWORD;
typedef struct {
    int maxdis;
    int dir;
	set<int> my_at;
	set<int> my_at_4;
	map<int, map<int, vector<vector<int> > > > distance_path_map;
}Next_in_4;
int findPosVector(vector <int> &input, int number);
bool is_element_in_vector(vector<int> &v, int element) ;
bool is_2_element_in_vector(vector<int> &v, int element1,int element2) ;
bool is_3_element_in_vector(vector<int> &v, int element1,int element2,int element3) ;
class seek_ring
{
public:
	int Vnum;
	int ringnum;
	string InFile;
	set<int> visit;
	queue<int> wait_4_solved;
	vector<int> my_out;
	vector<int> my_in;
	vector<int> path;//保留的探索path
	vector<DWORD> id;
	vector<DWORD> records;
	vector<vector<int> > nextedge;
	vector<vector<int> > fdedge;
	vector<Next_in_4> N_4_table;
	map<int, set<vector<int> > > ring_map;
	unordered_map<DWORD, int> id_2_index;

    


	seek_ring();
	void readFile();
	void InitGraph();
	void paixu(vector<int> &v1, vector<int> &v2);
    void paixu2(vector<int> &v1, vector<int> &v2);
	void DeleteVertex(int start);
	void check_0_degree(vector<int> &vc);
	void writeResult();
    void StartNext_4(int start, bool &va, int &i, int &now, int &nowdepth);//start
    void Solve_fd_3(int now, int &nowdepth, int &index);
    void Solve_fd_4(int now,bool &va, int &second, int &nowdepth, int &index);
    void Solve_next_4(int now, bool &va, int &second,int &nowdepth, int &index);
	void seek_ring_version2(vector<int> &vc);
};
seek_ring::seek_ring()
{
	ring_map[3] = set<vector<int>>{};
	ring_map[4] = set<vector<int>>{};
	ring_map[5] = set<vector<int>>{};
	ring_map[6] = set<vector<int>>{};
	ring_map[7] = set<vector<int>>{};
	ringnum = 0;
}
void seek_ring::readFile()
{
	{
		FILE* file = fopen(InFile.c_str(), "r");
		DWORD InId, OutId, Acount;
		int vi = 0;
		while (fscanf(file, "%u,%u,%u", &InId, &OutId, &Acount) != EOF) {
			records.push_back(InId);
			records.push_back(OutId);
		}
	}
}
void seek_ring::InitGraph()
{
	vector<DWORD> v1(records);
	sort(v1.begin(), v1.end());
	v1.erase(unique(v1.begin(), v1.end()), v1.end());
	Vnum = v1.size();
	//cout<<Vnum<<endl;
	id = v1;
	int index = 0;
	for (auto it1 = v1.begin(); it1 != v1.end(); ++it1) {
		id_2_index[*it1] = index;
		++index;
	}
	//cout<<Vnum<<endl;
	index = 0;
	nextedge=vector<vector<int>>(Vnum);
	fdedge=vector<vector<int>>(Vnum);
    N_4_table=vector<Next_in_4>(Vnum);
	for(int i = 0;i<Vnum;++i)
	{
		my_out.push_back(0);
		my_in.push_back(0);
	}
	for (; index < records.size(); index += 2) {
		int in_index = id_2_index[records[index]], out_index = id_2_index[records[index + 1]];
		nextedge[in_index].push_back(out_index);
		fdedge[out_index].push_back(in_index);
		my_out[in_index]++;
		my_in[out_index]++;
	}
}
void seek_ring::paixu(vector<int> &v1, vector<int> &v2)
{
	int min = 0, size = v1.size();
	vector<int> v3;

	for (int i = 1; i < size; ++i) {
		if (id[v1[i]]< id[v1[min]]) min = i;
	}
	for (int i = min; i < size; i++) v2.push_back(id[v1[i]]);
	for (int i = 0; i < min; i++) v2.push_back(id[v1[i]]);
}
void seek_ring::paixu2(vector<int> &v1, vector<int> &v2)
{
	int min = 0, size = v1.size();
	vector<int> v3;

	for (int i = 1; i < size; ++i) {
		if (id[v1[i]]< id[v1[min]]) min = i;
	}
	for (int i = min; i >=0; i--) v2.push_back(id[v1[i]]);
	for (int i = size - 1; i > min; i--) v2.push_back(id[v1[i]]);
}
void seek_ring::DeleteVertex(int index)
{
	int n_index;
	//for (Edge *edge = G.Vertex[index].fd_HeadEdge->Next; edge != NULL; edge = edge->Next) {
	for (auto it = fdedge[index].begin();it!= fdedge[index].end();++it) {
		n_index = *it;
		//--my_out[n_index];
		nextedge[n_index].erase(nextedge[n_index].begin() + findPosVector(nextedge[n_index], index));
	}
	for (auto it = nextedge[index].begin(); it != nextedge[index].end(); ++it) {
		n_index = *it;
		//--my_in[n_index];
		fdedge[n_index].erase(fdedge[n_index].begin() + findPosVector(fdedge[n_index], index));
	}
}
void seek_ring::check_0_degree(vector<int> &vc)
{
	for(auto n:vc){
		if(my_out[n]==0||my_in[n]==0){
			//cout<<"test"<<endl;
			DeleteVertex(n);
			vc.erase(vc.begin() + findPosVector(vc, n));
		}
	}
}
void seek_ring::writeResult()
{
	//ofstream ofile("/projects/student/result.txt");
	ofstream ofile("result.txt");
	int id, length = 3;
	
	if (!ofile.fail())
	{
		ofile << ringnum << endl;
		for (set<vector<int> >::iterator it = ring_map[3].begin(); it != ring_map[3].end(); ++it) {
			vector<int> link = (*it);
			vector<int>::iterator it2 = link.begin();
			ofile<<(*it2);
			++it2;
			for (; it2 != link.end(); ++it2) {
				ofile << ',' << (*it2) ;
			}
			ofile << '\n';
		}
		for (set<vector<int> >::iterator it = ring_map[4].begin(); it != ring_map[4].end(); ++it) {
			vector<int> link = (*it);
			vector<int>::iterator it2 = link.begin();
			ofile<<(*it2);
			++it2;
			for (; it2 != link.end(); ++it2) {
				ofile << ',' << (*it2) ;
			}
			ofile << '\n';
		}
		for (set<vector<int> >::iterator it = ring_map[5].begin(); it != ring_map[5].end(); ++it) {
			vector<int> link = (*it);
			vector<int>::iterator it2 = link.begin();
			ofile<<(*it2);
			++it2;
			for (; it2 != link.end(); ++it2) {
				ofile << ',' << (*it2) ;
			}
			ofile << '\n';
		}
		for (set<vector<int> >::iterator it = ring_map[6].begin(); it != ring_map[6].end(); ++it) {
			vector<int> link = (*it);
			vector<int>::iterator it2 = link.begin();
			ofile<<(*it2);
			++it2;
			for (; it2 != link.end(); ++it2) {
				ofile << ',' << (*it2) ;
			}
			ofile << '\n';
		}
		for (set<vector<int> >::iterator it = ring_map[7].begin(); it != ring_map[7].end(); ++it) {
			vector<int> link = (*it);
			vector<int>::iterator it2 = link.begin();
			ofile<<(*it2);
			++it2;
			for (; it2 != link.end(); ++it2) {
				ofile << ',' << (*it2) ;
			}
			ofile << '\n';
		}
	}
	ofile.close();
}
void seek_ring::StartNext_4(int start, bool &va, int &second, int &now, int &nowdepth)
{
	//cout<<"jinru start_n_4"<<endl;
    int n_index;
	bool bl = false;
	path.push_back(now);
	nowdepth++;
	if (nowdepth > 1) {
		if (va&&visit.count(now)) { va = false; bl = true; }
		if (N_4_table[start].my_at.count(now))
		{
			if (N_4_table[start].distance_path_map[now].count(nowdepth)) {
				N_4_table[start].distance_path_map[now][nowdepth].push_back(path);
			}
			else {
				vector<vector<int> > v2;
				v2.push_back(path);
				N_4_table[start].distance_path_map[now][nowdepth] = v2;
			}
		}
		else {
			N_4_table[start].my_at.insert(now);
			vector<vector<int> > v1;
			v1.push_back(path);
			map<int, vector<vector<int> > > map1;
			map1[nowdepth] = v1;
			N_4_table[start].distance_path_map[now] = map1;
		}
		if (va) {
			//fix mask
			if(nowdepth > 2){
			//fix :  juli wei 1dewanquan buyongjilu
				if (N_4_table[second].my_at.count(now))
			{
				if (N_4_table[second].distance_path_map[now].count(nowdepth - 1)) {
					N_4_table[second].distance_path_map[now][nowdepth - 1].push_back(vector<int> {path.begin() + 1, path.end()});
				}
				else {
					vector<vector<int> > v2;
					v2.push_back(vector<int> {path.begin() + 1, path.end()});
					N_4_table[second].distance_path_map[now][nowdepth - 1] = v2;
				}
			}
			else {
				N_4_table[second].my_at.insert(now);
				vector<vector<int> > v1;
				//vector<int> n_path(path);
				//n_path.erase(n_path.begin());
				//v1.push_back(n_path);
				v1.push_back(vector<int>{path.begin()+1,path.end()});
				//fix mask
				map<int, vector<vector<int> > > map1;
				map1[nowdepth - 1] = v1;
				N_4_table[second].distance_path_map[now] = map1;
			}
			}
		}
	}
	if (nowdepth == 1) {
		second = now;
        N_4_table[second].maxdis = 3;
        N_4_table[second].dir = 1;
        wait_4_solved.push(second);
		//fix :   wanquan   buyong  jilu
		/*
		if (N_4_table[start].my_at.count(now))
		{
			if (N_4_table[start].distance_path_map[now].count(1)) {
				N_4_table[start].distance_path_map[now][1].push_back(path);
			}
			else {
				vector<vector<int> > v2;
				v2.push_back(path);
				N_4_table[start].distance_path_map[now][1] = v2;
			}
		}
		else {
			N_4_table[start].my_at.insert(now);
			vector<vector<int> > v1;
			v1.push_back(path);
			map<int, vector<vector<int> > > map1;
			map1[1] = v1;
			N_4_table[start].distance_path_map[now] = map1;
		}
		*/
		visit.insert(now);
	}


	for (auto it = nextedge[now].begin(); it != nextedge[now].end(); ++it) {
		n_index = *it;
		if (!is_element_in_vector(path, n_index)) {
			if (nowdepth == 3) {
				if (!N_4_table[start].my_at_4.count(n_index))
					N_4_table[start].my_at_4.insert(n_index);
				if (N_4_table[start].my_at.count(n_index))
				{
					if (N_4_table[start].distance_path_map[n_index].count(4)) {
						vector<int> n_path(path);
						n_path.push_back(n_index);
						N_4_table[start].distance_path_map[n_index][4].push_back(n_path);
					}
					else {
						vector<vector<int> > v2;
						vector<int> n_path(path);
						n_path.push_back(n_index);
						v2.push_back(n_path);
						N_4_table[start].distance_path_map[n_index][4] = v2;
					}
				}
				else {
					N_4_table[start].my_at.insert(n_index);
					vector<vector<int> > v1;
					vector<int> n_path(path);
					n_path.push_back(n_index);
					v1.push_back(n_path);
					map<int, vector<vector<int> > > map1;
					map1[4] = v1;
					N_4_table[start].distance_path_map[n_index] = map1;
				}
				if (va) {
					if (!visit.count(n_index)) {
						if (!N_4_table[second].my_at_4.count(n_index))
									N_4_table[second].my_at_4.insert(n_index);
						if (N_4_table[second].my_at.count(n_index))
						{
							if (N_4_table[second].distance_path_map[n_index].count(3)) {
								//vector<int> n_path(path);
								//n_path.push_back(n_index);
								//n_path.erase(n_path.begin());
								vector<int> n_path{path.begin()+1,path.end()};
								n_path.push_back(n_index);
								//fix mask
								N_4_table[second].distance_path_map[n_index][3].push_back(n_path);
							}
							else {
								vector<vector<int> > v2;
								//vector<int> n_path(path);
								//n_path.push_back(n_index);
								//n_path.erase(n_path.begin());
								vector<int> n_path{path.begin()+1,path.end()};
								n_path.push_back(n_index);
								//fix mask
								v2.push_back(n_path);
								N_4_table[second].distance_path_map[n_index][3] = v2;
							}
						}
						else {
							N_4_table[second].my_at.insert(n_index);
							vector<vector<int> > v1;
							//vector<int> n_path(path);
							//n_path.push_back(n_index);
							//n_path.erase(n_path.begin());
							vector<int> n_path{path.begin()+1,path.end()};
							n_path.push_back(n_index);
							//fix mask
							v1.push_back(n_path);
							map<int, vector<vector<int> > > map1;
							map1[3] = v1;
							N_4_table[second].distance_path_map[n_index] = map1;
						}
						
					}

				}
			}
			//else DFS_next_limit_explore(start, va, i, n_index, nowdepth);
            else StartNext_4(start, va, second, n_index, nowdepth);
		}
	}
	if (bl == true) va = true;
	--nowdepth;
	path.pop_back();
	//cout<<"chuqu start_n_4"<<endl;
}
void seek_ring::Solve_fd_3(int now, int &nowdepth, int &index)
{
	//cout<<"jinru fd_3"<<endl;
    int n_index, length;
	path.push_back(now);
	++nowdepth;
	if (N_4_table[index].my_at.count(now)) {
		if (nowdepth == 1) {
			map<int, vector<vector<int> > > m1 = N_4_table[index].distance_path_map[now];
			map<int, vector<vector<int> > >::iterator it1 = m1.begin();
			while (it1 != m1.end()) {
				//fix   mask
				//if (it1->first == 1) { it1++; continue; }
				//jiran   1 meiyou renhe zuoyong    ,jiuwanquan buyong jilu  ta
				vector<vector<int> >::iterator v_it = it1->second.begin();
				while (v_it != it1->second.end()) {
					length = (*v_it).size();
					vector<int> ring;
					paixu(*v_it, ring);
					ring_map[length].insert(ring);
					++ringnum;
					++v_it;
				}
				++it1;
			}
		}
		if (nowdepth == 2) {
			if (N_4_table[index].my_at_4.count(now)) {
				vector<vector<int> >::iterator v_it = N_4_table[index].distance_path_map[now][4].begin();
				while (v_it != N_4_table[index].distance_path_map[now][4].end()) {
					if (!is_element_in_vector((*v_it), path[1]))
					{
						vector<int> link = (*v_it);
						link.push_back(path[1]);
						length = (link).size();
						vector<int> ring;
						paixu(link, ring);
						ring_map[length].insert(ring);
						++ringnum;
					}
					++v_it;
				}
			}
		}

	}

	//for (Edge *edge = G.Vertex[now].fd_HeadEdge->Next; edge != NULL; edge = edge->Next) {
	for (auto it = fdedge[now].begin();it!= fdedge[now].end();++it) {
		n_index = *it;
		if (!is_element_in_vector(path, n_index)) {
			if (nowdepth == 2) {
				if (N_4_table[index].distance_path_map[n_index].count(4)) {
					vector<vector<int> > v1 = N_4_table[index].distance_path_map[n_index][4];
					vector<vector<int> >::iterator v_it = v1.begin();
					while (v_it != v1.end()) {
						//fix:genghuan     2
						//if (!is_element_in_vector((*v_it), path[1]) && !is_element_in_vector((*v_it), path[2])) {
							//fix mask
						if(!is_2_element_in_vector((*v_it),path[1],path[2])){
							vector<int> link = (*v_it);
							link.push_back(path[2]);
							link.push_back(path[1]);
							length = link.size();
							vector<int> ring;
							paixu(link, ring);
							ring_map[length].insert(ring);
							++ringnum;
						}
						++v_it;
					}
				}
			}
			else Solve_fd_3(n_index, nowdepth, index);
		}
	}
	--nowdepth;
	path.pop_back();
	//cout<<"chuqu fd_3"<<endl;
}
void seek_ring::Solve_fd_4(int now, bool &va, int &second,int &nowdepth, int &index)
{
	//cout<<"jinru fd_4"<<endl;
     int n_index, length;
     bool bl = false;
	path.push_back(now);
	++nowdepth;
    if(nowdepth == 1)
    {
         second = now;
		if(!visit.count(second))
		{N_4_table[second].maxdis = 3;
        N_4_table[second].dir = 0;
        wait_4_solved.push(second);visit.insert(now);
		}
		else{
			va = false;bl=true;
		}
        if (N_4_table[index].my_at.count(now))
        {
            map<int, vector<vector<int> > > m1 = N_4_table[index].distance_path_map[now];
			map<int, vector<vector<int> > >::iterator it1 = m1.begin();
			while (it1 != m1.end()) {
				//if (it1->first == 1) { it1++; continue; }
				vector<vector<int> >::iterator v_it = it1->second.begin();
				while (v_it != it1->second.end()) {
					length = (*v_it).size();
					vector<int> ring;
					paixu(*v_it, ring);
					ring_map[length].insert(ring);
					++ringnum;
					++v_it;
				}
				++it1;
			}
        }
    }
    if(nowdepth == 2)
    {
        if (va&&visit.count(now)) { va = false; bl = true; }
		//fix :wanquan  buyong   jilu
		/*
        if (va) {
			if (N_4_table[second].my_at.count(now))
			{
				if (N_4_table[second].distance_path_map[now].count(nowdepth - 1)) {
					N_4_table[second].distance_path_map[now][nowdepth - 1].push_back(vector<int> {path.begin() + 1, path.end()});
				}
				else {
					vector<vector<int> > v2;
					v2.push_back(vector<int> {path.begin() + 1, path.end()});
					N_4_table[second].distance_path_map[now][nowdepth - 1] = v2;
				}
			}
			else {
				N_4_table[second].my_at.insert(now);
				vector<vector<int> > v1;
				vector<int> n_path(path);
				n_path.erase(n_path.begin());
				v1.push_back(n_path);
				map<int, vector<vector<int> > > map1;
				map1[nowdepth - 1] = v1;
				N_4_table[second].distance_path_map[now] = map1;
			}
			
		}
		*/
        /*fenge*/
        if (N_4_table[index].my_at_4.count(now)) {
			vector<vector<int> >::iterator v_it = N_4_table[index].distance_path_map[now][3].begin();
			while (v_it != N_4_table[index].distance_path_map[now][3].end()) {
				if (!is_element_in_vector((*v_it), path[1]))
				{
					vector<int> link = (*v_it);
					link.push_back(path[1]);
					length = (link).size();
					vector<int> ring;
					paixu(link, ring);
					ring_map[length].insert(ring);
					++ringnum;
				}
				++v_it;
			}
		}
    }
    if(nowdepth == 3){
        if (va&&visit.count(now)) { va = false; bl = true; }
        if (va) {
			if (N_4_table[second].my_at.count(now))
			{
				if (N_4_table[second].distance_path_map[now].count(nowdepth - 1)) {
					N_4_table[second].distance_path_map[now][nowdepth - 1].push_back(vector<int> {path.begin() + 1, path.end()});
				}
				else {
					vector<vector<int> > v2;
					v2.push_back(vector<int> {path.begin() + 1, path.end()});
					N_4_table[second].distance_path_map[now][nowdepth - 1] = v2;
				}
			}
			else {
				N_4_table[second].my_at.insert(now);
				vector<vector<int> > v1;
				//vector<int> n_path(path);
				//n_path.erase(n_path.begin());
				vector<int> n_path(path.begin()+1,path.end());
				//fix mask
				v1.push_back(n_path);
				map<int, vector<vector<int> > > map1;
				map1[nowdepth - 1] = v1;
				N_4_table[second].distance_path_map[now] = map1;
			}
		}
        /*fenge*/
        if (N_4_table[index].my_at_4.count(now)) {
				vector<vector<int> >::iterator v_it = N_4_table[index].distance_path_map[now][3].begin();
				while (v_it != N_4_table[index].distance_path_map[now][3].end()) {
					//if (!is_element_in_vector((*v_it), path[1]) && !is_element_in_vector((*v_it), path[2]))
					//fix   mask
					if(!is_2_element_in_vector((*v_it), path[1],path[2]))
					{
						vector<int> link = (*v_it);
                        link.push_back(path[2]);
						link.push_back(path[1]);
						length = link.size();
						vector<int> ring;
						paixu(link, ring);
						ring_map[length].insert(ring);
						++ringnum;
					}
					++v_it;
				}
			}
    }
	//for (Edge *edge = G.Vertex[now].fd_HeadEdge->Next; edge != NULL; edge = edge->Next) {
	for (auto it = fdedge[now].begin();it!= fdedge[now].end();++it) {
		n_index = *it;
		if (!is_element_in_vector(path, n_index)) {
			if (nowdepth == 3) {
				if (N_4_table[index].distance_path_map[n_index].count(3)) {
					vector<vector<int> > v1 = N_4_table[index].distance_path_map[n_index][3];
					vector<vector<int> >::iterator v_it = v1.begin();
					while (v_it != v1.end()) {
						//if (!is_element_in_vector((*v_it), path[1]) && !is_element_in_vector((*v_it), path[2])&&!is_element_in_vector((*v_it), path[3])) {
							//fix mask
						if(!is_3_element_in_vector((*v_it), path[1],path[2],path[3]))
						{
							vector<int> link = (*v_it);
                            link.push_back(path[3]);
							link.push_back(path[2]);
							link.push_back(path[1]);
							length = link.size();
							vector<int> ring;
							paixu(link, ring);
							ring_map[length].insert(ring);
							++ringnum;
						}
						++v_it;
					}
				}
                /*fenge*/
                if (va) {
					if (!visit.count(n_index)) {
						if (!N_4_table[second].my_at_4.count(n_index))
									N_4_table[second].my_at_4.insert(n_index);
						if (N_4_table[second].my_at.count(n_index))
						{
							if (N_4_table[second].distance_path_map[n_index].count(3)) {
								//vector<int> n_path(path);
								//n_path.erase(n_path.begin());
								vector<int> n_path(path.begin()+1,path.end());
								//fix mask
								n_path.push_back(n_index);
								
								N_4_table[second].distance_path_map[n_index][3].push_back(n_path);
							}
							else {
								vector<vector<int> > v2;
								//vector<int> n_path(path);
								//n_path.erase(n_path.begin());
								vector<int> n_path(path.begin()+1,path.end());
								//fix mask
								n_path.push_back(n_index);
								v2.push_back(n_path);
								N_4_table[second].distance_path_map[n_index][3] = v2;
							}
						}
						else {
							N_4_table[second].my_at.insert(n_index);
							vector<vector<int> > v1;
							//vector<int> n_path(path);
							//n_path.erase(n_path.begin());
							vector<int> n_path(path.begin()+1,path.end());
							//fix mask
							n_path.push_back(n_index);
							v1.push_back(n_path);
							map<int, vector<vector<int> > > map1;
							map1[3] = v1;
							N_4_table[second].distance_path_map[n_index] = map1;
						}
						
					}

				}
			}
			else Solve_fd_4(n_index, va,second,nowdepth, index);
		}
	}
	if (bl == true) va = true;
	--nowdepth;
	path.pop_back();
	//cout<<"chuqu fd_4"<<endl;
}
void seek_ring::Solve_next_4(int now, bool &va, int &second,int &nowdepth, int &index)
{
	//cout<<"jinru next_4"<<endl;
     int n_index, length;
     bool bl = false;
	path.push_back(now);
	++nowdepth;
    if(nowdepth == 1)
    {
         second = now;
		 if(!visit.count(second))
		{N_4_table[second].maxdis = 3;
        N_4_table[second].dir = 1;
        wait_4_solved.push(second);visit.insert(now);
		}
		else{
			va = false;bl=true;
		}
        if (N_4_table[index].my_at.count(now))
        {
            map<int, vector<vector<int> > > m1 = N_4_table[index].distance_path_map[now];
			map<int, vector<vector<int> > >::iterator it1 = m1.begin();
			while (it1 != m1.end()) {
				//if (it1->first == 1) { it1++; continue; }
				vector<vector<int> >::iterator v_it = it1->second.begin();
				while (v_it != it1->second.end()) {
					length = (*v_it).size();
					vector<int> ring;
					paixu2(*v_it, ring);
					ring_map[length].insert(ring);
					++ringnum;
					++v_it;
				}
				++it1;
			}
        }
    }
    if(nowdepth == 2)
    {
        if (va&&visit.count(now)) { va = false; bl = true; }
		//fix:wanquan buyong jilu
		/*
        if (va) {
			if (N_4_table[second].my_at.count(now))
			{
				if (N_4_table[second].distance_path_map[now].count(1)) {
					N_4_table[second].distance_path_map[now][1].push_back(vector<int> {path.begin() + 1, path.end()});
				}
				else {
					vector<vector<int> > v2;
					v2.push_back(vector<int> {path.begin() + 1, path.end()});
					N_4_table[second].distance_path_map[now][1] = v2;
				}
			}
			else {
				N_4_table[second].my_at.insert(now);
				vector<vector<int> > v1;
				//vector<int> n_path(path);
				//n_path.erase(n_path.begin());
				vector<int> n_path{path.begin()+1,path.end()};
				v1.push_back(n_path);
				map<int, vector<vector<int> > > map1;
				map1[1] = v1;
				N_4_table[second].distance_path_map[now] = map1;
			}
		}
		*/
        /*fenge*/
         if (N_4_table[index].my_at_4.count(now)) {
			vector<vector<int> >::iterator v_it = N_4_table[index].distance_path_map[now][3].begin();
			while (v_it != N_4_table[index].distance_path_map[now][3].end()) {
				if (!is_element_in_vector((*v_it), path[1]))
				{
					vector<int> link = (*v_it);
					link.push_back(path[1]);
					length = (link).size();
					vector<int> ring;
					paixu2(link, ring);
					ring_map[length].insert(ring);
					++ringnum;
				}
				++v_it;
			}
		}
    }
    if(nowdepth == 3){
        if (va&&visit.count(now)) { va = false; bl = true; }
        if (va) {
			if (N_4_table[second].my_at.count(now))
			{
				if (N_4_table[second].distance_path_map[now].count(2)) {
					N_4_table[second].distance_path_map[now][2].push_back(vector<int> {path.begin() + 1, path.end()});
				}
				else {
					vector<vector<int> > v2;
					v2.push_back(vector<int> {path.begin() + 1, path.end()});
					N_4_table[second].distance_path_map[now][2] = v2;
				}
			}
			else {
				N_4_table[second].my_at.insert(now);
				vector<vector<int> > v1;
				v1.push_back(vector<int>{path.begin()+1,path.end()});
				map<int, vector<vector<int> > > map1;
				map1[2] = v1;
				N_4_table[second].distance_path_map[now] = map1;
			}
		}
        /*fenge*/
        if (N_4_table[index].my_at_4.count(now)) {
				vector<vector<int> >::iterator v_it = N_4_table[index].distance_path_map[now][3].begin();
				while (v_it != N_4_table[index].distance_path_map[now][3].end()) {
					//if (!is_element_in_vector((*v_it), path[1]) && !is_element_in_vector((*v_it), path[2]))
					//fix mask
					if(!is_2_element_in_vector((*v_it), path[1],path[2]))
					{
						vector<int> link = (*v_it);
                        link.push_back(path[2]);
						link.push_back(path[1]);
						length = link.size();
						vector<int> ring;
						paixu2(link, ring);
						ring_map[length].insert(ring);
						++ringnum;
					}
					++v_it;
				}
			}
    }
	//for (Edge *edge = G.Vertex[now].fd_HeadEdge->Next; edge != NULL; edge = edge->Next) {
	for (auto it = nextedge[now].begin();it!= nextedge[now].end();++it) {
		n_index = *it;
		if (!is_element_in_vector(path, n_index)) {
			if (nowdepth == 3) {
				if (N_4_table[index].distance_path_map[n_index].count(3)) {
					vector<vector<int> > v1 = N_4_table[index].distance_path_map[n_index][3];
					vector<vector<int> >::iterator v_it = v1.begin();
					while (v_it != v1.end()) {
						//if (!is_element_in_vector((*v_it), path[1]) && !is_element_in_vector((*v_it), path[2])&&!is_element_in_vector((*v_it), path[3])) {
						if(!is_3_element_in_vector((*v_it), path[1],path[2],path[3]))
						{
							vector<int> link = (*v_it);
                            link.push_back(path[3]);
							link.push_back(path[2]);
							link.push_back(path[1]);
							length = link.size();
							vector<int> ring;
							paixu2(link, ring);
							ring_map[length].insert(ring);
							++ringnum;
						}
						++v_it;
					}
				}
                /*fenge*/
                if (va) {
					if (!visit.count(n_index)) {
						if (!N_4_table[second].my_at_4.count(n_index))
									N_4_table[second].my_at_4.insert(n_index);
						if (N_4_table[second].my_at.count(n_index))
						{
							if (N_4_table[second].distance_path_map[n_index].count(3)) {
								//vector<int> n_path(path);
								//n_path.erase(n_path.begin());
								vector<int> n_path{path.begin()+1,path.end()};
								//fix mask
								n_path.push_back(n_index);
								N_4_table[second].distance_path_map[n_index][3].push_back(n_path);
							}
							else {
								vector<vector<int> > v2;
								//vector<int> n_path(path);
								//n_path.erase(n_path.begin());
								vector<int> n_path{path.begin()+1,path.end()};
								//fix mask
								n_path.push_back(n_index);
								v2.push_back(n_path);
								N_4_table[second].distance_path_map[n_index][3] = v2;
							}
						}
						else {
							N_4_table[second].my_at.insert(n_index);
							vector<vector<int> > v1;
							//vector<int> n_path(path);
							//n_path.erase(n_path.begin());
							vector<int> n_path{path.begin()+1,path.end()};
							//fix mask
							n_path.push_back(n_index);
							v1.push_back(n_path);
							map<int, vector<vector<int> > > map1;
							map1[3] = v1;
							N_4_table[second].distance_path_map[n_index] = map1;
						}
						
					}

				}
			}
			else Solve_next_4(n_index,va,second, nowdepth, index);
		}
	}
	if (bl == true) va = true;
	--nowdepth;
	path.pop_back();
	//cout<<"chuqu next_4"<<endl;
}
void seek_ring::seek_ring_version2(vector<int> &vc)
{
    if (vc.size() < 3) return;
	int nowdepth = -1, start, maxdis,dir,second = 0;
	bool va = true;
	int i = 0;
    start = vc[0];
	check_0_degree(vc);
	//cout<<"que size:"<<wait_4_solved.size()<<endl;
    wait_4_solved.push(start);

    visit.insert(start);
    //cout<<"start"<<start<<endl;
    N_4_table[start].dir=1;
    N_4_table[start].maxdis=4;
	//cout<<"chudu"<<my_out[start]<<endl;
    StartNext_4(start,va,second,start,nowdepth);
	//cout<<"que size:"<<wait_4_solved.size()<<endl;
	while (vc.size()) {
        if(vc.size()<3) break;
		while(!wait_4_solved.empty()){
			//check_0_degree(vc);
			//cout<<"que size:"<<wait_4_solved.size()<<endl;
            start = wait_4_solved.front();
            wait_4_solved.pop();
            dir = N_4_table[start].dir;
            maxdis = N_4_table[start].maxdis;
            va = true;
            nowdepth = -1;
            second = 0;
            //cout<<dir<<endl;
            //cout<<maxdis<<endl;
            if(dir == 1&&maxdis == 3) 
			{//cout<<"yongle: fd_4"<<endl;
				Solve_fd_4(start, va,second,nowdepth,start);}
            else if(dir == 1&&maxdis == 4) 
			{//cout<<"yongle: fd_3"<<endl;
			Solve_fd_3(start,nowdepth,start);}
            else if(dir ==0&&maxdis==3) 
			{//cout<<"yongle: next_4"<<endl;
			Solve_next_4(start,va,second,nowdepth,start);}
			//else{cout<<"ssssssssss"<<endl;}


			N_4_table[start].my_at.clear();
			N_4_table[start].my_at_4.clear();
			N_4_table[start].distance_path_map.clear();
            DeleteVertex(start);
            vc.erase(vc.begin() + findPosVector(vc, start));
            cout<<"shengyudianshu:"<<vc.size()<<endl;
        }
        //cout<<"test1"<<endl;
        start =vc[0];
        wait_4_solved.push(vc[0]);
        N_4_table[start].dir=1;
        N_4_table[start].maxdis=4;
        nowdepth = -1;
        visit.insert(vc[0]);
        StartNext_4(vc[0],va,second,vc[0],nowdepth);
	}
}
bool is_element_in_vector(vector<int> &v, int element) 
{
	vector<int>::iterator it = v.begin();
	while (it != v.end())
	{
		if ((*it) == element)return true;
		++it;
	}
	return false;
}
bool is_2_element_in_vector(vector<int> &v, int element1,int element2) 
//zhiyao you   yige  zai  ,jiu renturn  true
{
	vector<int>::iterator it = v.begin();
	while (it != v.end())
	{
		if((*it)==element1||(*it)==element2) return true;
		++it;
	}
	return false;
}
bool is_3_element_in_vector(vector<int> &v, int element1,int element2,int element3) 
//zhiyao you   yige  zai  ,jiu renturn  true
{
	vector<int>::iterator it = v.begin();
	while (it != v.end())
	{
		if((*it)==element1||(*it)==element2||(*it)==element3) return true;
		++it;
	}
	return false;
}
int findPosVector(vector <int> &input, int number)
{
	vector<int>::iterator iter = input.begin();
	int len = 0;
	while (iter != input.end()) {
		if ((*iter) == number) return len;
		++len;
		++iter;
	}


}
int main()
{
	//time_t starttime, endtime, starttime2;
	//starttime = clock();
	//double duration;
	seek_ring solution;
	//string filepath = "test_data_38252.txt";
	//string filepath = "test_data_77409.txt";
	//string filepath = "test_data_1004812.txt";
	//string filepath = "test_data_3512444.txt";
	//string filepath = "test_data_2896262.txt";
	string filepath ="/data/test_data.txt";
	solution.InFile = filepath;
	solution.readFile();
	solution.InitGraph();
	vector<int> vc;
	for (int i = 0; i < solution.Vnum; ++i) {
		vc.push_back(i);
	}
    //cout<<"s"<<solution.Vnum<<endl;
	solution.seek_ring_version2(vc);
	solution.writeResult();
	//endtime = clock();
	//duration = (endtime - starttime) / CLOCKS_PER_SEC;
	//cout << "环数：" << solution.ringnum << endl;
	//cout << "用时：" << duration <<'s'<<endl;
    //std::cout << "Hello World!\n";
	return 0;
}
