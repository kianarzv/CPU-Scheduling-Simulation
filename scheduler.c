#include "scheduler.h"
#include <stdlib.h>
#include <string.h>

// FCFS 
Metrics fcfs_metrics(Process proc[], int n) {
    Process *p = malloc(n * sizeof(Process));
    memcpy(p, proc, n * sizeof(Process));
    int time = 0;
    float totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;
    for (int i = 0; i < n; i++) {
        if (time < p[i].arrivalTime) {
            time = p[i].arrivalTime;
        }
        p[i].startTime = time;
        time += p[i].burstTime;
        p[i].completionTime = time;

        int turnaround = p[i].completionTime - p[i].arrivalTime;
        int waiting = turnaround - p[i].burstTime;
        int response = p[i].startTime - p[i].arrivalTime;

        totalTurnaround += turnaround;
        totalWaiting += waiting;
        totalResponse += response;
    }

    free(p);
    Metrics m = {
        totalTurnaround / n,
        totalWaiting / n,
        totalResponse / n
    };
    return m;
}

