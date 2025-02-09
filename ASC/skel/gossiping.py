"""
Implementati o propagare ciclica de tip gossiping folosind bariere. 
  * Se considera N noduri (obiecte de tip Thread), cu indecsi 0...N-1.
  * Fiecare nod tine o valoare generata random.
  * Calculati valoarea minima folosind urmatorul procedeu:
     * nodurile ruleaza in cicluri
     * intr-un ciclu, fiecare nod comunica cu un subset de alte noduri pentru a
       obtine valoarea acestora si a o compara cu a sa
       * ca subset considerati random 3 noduri din lista de noduri primita in
        constructor si obtineti valoarea acestora (metoda get_value)
     * dupa terminarea unui ciclu, fiecare nod va avea ca valoare minimul
       obtinut in ciclul anterior
     * la finalul iteratiei toate nodurile vor contine valoarea minima
  * Folositi una din barierele reentrante din modulul barrier.
  * Pentru a simplifica implementarea, e folosit un numar fix de cicluri,
    negarantand astfel convergenta tutoror nodurilor la acelasi minim.
"""

import sys
import random

from threading import Thread

from barrier import ReusableBarrierSem


random.seed(0) # genereaza tot timpul aceeasi secventa pseudo-random

class Node(Thread):
    #TODO Node trebuie sa fie Thread

    def __init__(self, node_id, all_nodes, num_iter, my_barrier):
        #TODO nodurile trebuie sa foloseasca un obiect bariera

        Thread.__init__(self)
        self.node_id = node_id
        self.all_nodes = all_nodes
        self.num_iter = num_iter
        self.value = random.randint(1, 1000)
        self.my_barrier = my_barrier
         
    def get_value(self):
        return self.value
    
    def run(self): 
      for i in range(self.num_iter):
        gossip_group = list()
        for j in range(3):
            gossip_group.append(all_nodes[random.randint(1, len(all_nodes) -1)])
        for k in gossip_group:
            if k.get_value() < self.value:
              self.value = k.get_value()
        my_barrier.wait()


if __name__ == "__main__":
    if len(sys.argv) == 2:
        num_threads = int(sys.argv[1])
    else:
        print("Usage: python " + sys.argv[0] + " num_nodes")
        sys.exit(0)
    
    num_iter = 10  # numar iteratii/cicluri algoritm

    my_barrier = ReusableBarrierSem(num_threads)
    all_nodes = list()
    all_nodes = [Node(x, all_nodes, num_iter, my_barrier) for x in range(num_threads)]
    for node in all_nodes:
      node.start()

    for node in all_nodes:
      node.join()
   
    for node in all_nodes:
        print("Node: " + str(node.node_id) + ", Value: " + str(node.get_value()))
    #TODO Create nodes and start them

    #TODO Wait for nodes to finish
