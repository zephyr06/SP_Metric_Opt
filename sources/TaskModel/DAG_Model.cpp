#include "sources/TaskModel/DAG_Model.h"

namespace SP_OPT_PA {

std::vector<int> shortest_paths(Vertex root, Vertex target, Graph const& g) {
    std::vector<int> path;
    // find shortest paths from the root
    std::vector<Vertex> predecessors(boost::num_vertices(g), NIL);
    auto recorder = boost::record_predecessors(predecessors.data(),
                                               boost::on_examine_edge());
    boost::breadth_first_search(
        g, root, boost::visitor(boost::make_bfs_visitor(recorder)));

    for (auto pred = predecessors[target]; pred != NIL;
         pred = predecessors[pred]) {
        path.push_back(pred);
    }
    if (path.size() != 0) {
        std::reverse(path.begin(), path.end());
        path.push_back(target);
    }
    return path;
}

void PrintChains(const std::vector<std::vector<int>>& chains) {
    std::cout << "Chains:" << std::endl;
    for (size_t i = 0; i < size(chains); i++) {
        for (size_t j = 0; j < size(chains[i]); j++) {
            std::cout << chains[i][j] << ", ";
        }
        std::cout << std::endl;
    }
}

// *2, 1 means task 2 depend on task 1, or task 1 must execute before task 2;
// 1 would be the first in MAP_Prev, while 2 is one task in TaskSet
// MAP_Prev maps one task to all the tasks it depends on

std::pair<Graph, indexVertexMap> DAG_Model::GenerateGraphForTaskSet() const {
    Graph g;
    // map to access properties of vertex from the graph
    vertex_name_map_t vertex2indexBig = get(boost::vertex_name, g);

    // map to access vertex from its global index
    indexVertexMap indexesBGL;
    for (uint i = 0; i < tasks.size(); i++) {
        indexVertexMap::iterator pos;
        bool inserted;
        Vertex u;
        boost::tie(pos, inserted) =
            indexesBGL.insert(std::make_pair(i, Vertex()));
        if (inserted) {
            u = add_vertex(g);
            vertex2indexBig[u] = i;
            pos->second = u;
        } else {
            CoutError("Error building indexVertexMap!");
        }
    }

    // add edges

    for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
        const TaskSet& tasksPrev =
            itr->second;  // task ids that must finish before
                          // the task whose id is indexNext
        size_t indexNext = itr->first;
        for (size_t i = 0; i < tasksPrev.size(); i++) {
            boost::add_edge(tasksPrev[i].id, GetTask(indexNext).id, g);
        }
    }
    return std::make_pair(g, indexesBGL);
}

void DAG_Model::print() {
    for (auto& task : tasks) task.print();
    for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
        for (uint i = 0; i < (itr->second).size(); i++)
            std::cout << "Edge: " << ((itr->second)[i].id) << "-->"
                      << (itr->first) << std::endl;
    }
}

void DAG_Model::printChains() {
    for (size_t i = 0; i < chains_.size(); i++) {
        std::cout << "Chain #" << i << ": ";
        for (auto task : chains_[i]) {
            std::cout << task << ", ";
        }
        std::cout << std::endl;
    }
}

int DAG_Model::edgeNumber() {
    int count = 0;
    for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
        count += (itr->second).size();
    }
    return count;
}

std::vector<int> DAG_Model::FindSourceTaskIds() const {
    std::set<int> originTasks;
    for (uint i = 0; i < tasks.size(); i++) {
        originTasks.insert(i);
    }

    for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
        size_t indexNext = itr->first;
        originTasks.erase(indexNext);
    }
    std::vector<int> res(originTasks.size());
    std::copy(originTasks.begin(), originTasks.end(), res.begin());
    return res;
}
std::vector<int> DAG_Model::FindSinkTaskIds() const {
    std::set<int> originTasks;
    for (uint i = 0; i < tasks.size(); i++) {
        originTasks.insert(i);
    }

    for (auto itr = mapPrev.begin(); itr != mapPrev.end(); itr++) {
        auto parents = itr->second;
        for (auto p : parents) {
            originTasks.erase(p.id);
        }
    }
    std::vector<int> res(originTasks.size());
    std::copy(originTasks.begin(), originTasks.end(), res.begin());
    return res;
}

std::vector<std::vector<int>> DAG_Model::GetRandomChains(int numOfChains,
                                                         int chain_length) {
    std::vector<std::vector<int>> chains;
    chains.reserve(numOfChains);
    int chainCount = 0;
    auto rng = std::default_random_engine{};
    std::vector<int> sourceIds = FindSourceTaskIds();
    std::vector<int> sinkIds = FindSinkTaskIds();

    std::shuffle(std::begin(sourceIds), std::end(sourceIds), rng);
    std::shuffle(std::begin(sinkIds), std::end(sinkIds), rng);

    for (int sourceId : sourceIds) {
        for (int sinkId : sinkIds) {
            if (chainCount >= numOfChains)
                break;
            auto path = shortest_paths(sourceId, sinkId, graph_);
            if (path.size() > 1) {
                if (chain_length == 0 ||
                    (chain_length > 0 && path.size() >= chain_length))
                    chains.push_back(path);
                chainCount++;
            }
        }
    }
    return chains;
}

void DAG_Model::CategorizeTaskSet() {
    for (uint i = 0; i < tasks.size(); i++) {
        int task_id = tasks[i].id;
        int p_id = tasks[i].processorId;
        auto itr = processor2taskset_.find(p_id);
        if (itr == processor2taskset_.end()) {
            processor2taskset_[p_id] = {tasks[i]};
        } else {
            processor2taskset_[p_id].push_back(tasks[i]);
        }
        task_id2task_index_within_processor_[task_id] =
            processor2taskset_[p_id].size() - 1;
    }
}
void DAG_Model::RecordTaskPosition() {
    for (int i = 0; i < static_cast<int>(tasks.size()); i++) {
        task_id2position_[tasks[i].id] = i;
    }
}

// transform a string to a vectof of int
std::vector<int> Str2VecInt(const std::string& str) {
    std::vector<int> vect;
    std::stringstream ss(str);
    for (int i; ss >> i;) {
        vect.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }
    return vect;
}

DAG_Model ReadDAG_Tasks(std::string path, int max_possible_chains) {
    TaskSet tasks = ReadTaskSet(path);
    YAML::Node config = YAML::LoadFile(path);
    std::vector<std::vector<int>> chains;
    std::vector<double> chains_deadlines;
    for (int i = 0; i < max_possible_chains; i++) {
        if (config["chain" + std::to_string(i)]) {
            YAML::Node chain_node = config["chain" + std::to_string(i)];
            if (chain_node["nodes"]) {
                std::vector<int> chain_curr =
                    chain_node["nodes"].as<std::vector<int>>();
                chains.push_back(chain_curr);
                if (chain_node["deadline"]) {
                    double deadline = chain_node["deadline"].as<double>();
                    chains_deadlines.push_back(deadline);
                } else
                    CoutError("No deadline in chain " + std::to_string(i) +
                              " in " + path);
            } else
                std::cout << "No nodes in " << path << std::endl;
        } else
            break;
    }
    return DAG_Model(tasks, chains, chains_deadlines);
}

void WriteDAG_Tasks_chains(std::string path, const DAG_Model& dag_tasks) {
    YAML::Node chains;
    for (uint i = 0; i < dag_tasks.chains_.size(); i++) {
        chains["chain" + std::to_string(i)]["nodes"] = dag_tasks.chains_[i];
        chains["chain" + std::to_string(i)]["deadline"] =
            dag_tasks.chains_deadlines_[i];
    }
    std::ofstream fout(path, std::ios_base::app | std::ios_base::out);
    fout << "\n\n";
    fout << chains;
    fout.close();
}

void WriteDAG_Tasks(std::string path, const DAG_Model& dag_tasks) {
    WriteTaskSet(path, dag_tasks.tasks);
    WriteDAG_Tasks_chains(path, dag_tasks);
}

bool WhetherDAGChainsShareNodes(const DAG_Model& dag_tasks) {
    const std::vector<std::vector<int>>& chains = dag_tasks.chains_;
    for (const auto& chain_curr : chains) {
        std::unordered_set<int> record_curr;
        record_curr.reserve(chain_curr.size());
        for (int x : chain_curr) record_curr.insert(x);
        for (const auto& chain_compare : chains) {
            if (chain_curr != chain_compare) {
                for (int y : chain_compare)
                    if (record_curr.find(y) != record_curr.end())
                        return true;
            }
        }
    }
    return false;
}

}  // namespace SP_OPT_PA