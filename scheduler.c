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

// SJF (Non-Preemptive) 
int cmp_sjf(const void *a, const void *b) {
    const Process *p1 = a, *p2 = b;
    if (p1->arrivalTime != p2->arrivalTime)
        return p1->arrivalTime - p2->arrivalTime;
    return p1->burstTime - p2->burstTime;
}

Metrics sjf_metrics(Process proc[], int n) {
    Process *p = malloc(n * sizeof(Process));
    memcpy(p, proc, n * sizeof(Process));

    int completed = 0, time = 0;
    int *done = calloc(n, sizeof(int));
    float totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;

    while (completed < n) {
        int idx = -1;
        int minBurst = 1e9;
        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrivalTime <= time && p[i].burstTime < minBurst) {
                minBurst = p[i].burstTime;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        p[idx].startTime = time;
        time += p[idx].burstTime;
        p[idx].completionTime = time;
        done[idx] = 1;
        completed++;

        int turnaround = p[idx].completionTime - p[idx].arrivalTime;
        int waiting = turnaround - p[idx].burstTime;
        int response = p[idx].startTime - p[idx].arrivalTime;

        totalTurnaround += turnaround;
        totalWaiting += waiting;
        totalResponse += response;
    }

    free(done);
    free(p);
    Metrics m = {
        totalTurnaround / n,
        totalWaiting / n,
        totalResponse / n
    };
    return m;
}

// Round Robin
Metrics rr_metrics(Process proc[], int n, int timeQuantum) {
    Process *p = malloc(n * sizeof(Process));
    memcpy(p, proc, n * sizeof(Process));

    int *remaining = malloc(n * sizeof(int));
    int *start = malloc(n * sizeof(int));
    int *started = calloc(n, sizeof(int));
    int time = 0, completed = 0;
    float totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;

    for (int i = 0; i < n; i++) {
        remaining[i] = p[i].burstTime;
        start[i] = -1;
    }

    int queue[100], front = 0, rear = 0;
    int inQueue[n];
    memset(inQueue, 0, sizeof(inQueue));

    while (completed < n) {
        // Enqueue arrived processes
        for (int i = 0; i < n; i++) {
            if (p[i].arrivalTime <= time && !inQueue[i] && remaining[i] > 0) {
                queue[rear++] = i;
                inQueue[i] = 1;
            }
        }

        if (front == rear) {
            time++;
            continue;
        }

        int idx = queue[front++];
        if (start[idx] == -1) {
            start[idx] = time;
            totalResponse += time - p[idx].arrivalTime;
        }

        int execTime = (remaining[idx] < timeQuantum) ? remaining[idx] : timeQuantum;
        remaining[idx] -= execTime;
        time += execTime;

        // Enqueue newly arrived during execution
        for (int i = 0; i < n; i++) {
            if (p[i].arrivalTime > time - execTime && p[i].arrivalTime <= time && !inQueue[i] && remaining[i] > 0) {
                queue[rear++] = i;
                inQueue[i] = 1;
            }
        }

        if (remaining[idx] > 0) {
            queue[rear++] = idx;
        } else {
            int turnaround = time - p[idx].arrivalTime;
            int waiting = turnaround - p[idx].burstTime;

            totalTurnaround += turnaround;
            totalWaiting += waiting;
            completed++;
        }
        inQueue[idx] = 0;
    }

    free(p);
    free(remaining);
    free(start);
    free(started);

    Metrics m = {
        totalTurnaround / n,
        totalWaiting / n,
        totalResponse / n
    };
    return m;
}
