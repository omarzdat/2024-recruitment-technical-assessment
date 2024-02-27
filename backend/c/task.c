#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_LEN 50
#define MAX_CATEGORIES 10

struct File {
    int id;
    char name[MAX_STRING_LEN];
    char categories[MAX_CATEGORIES][MAX_STRING_LEN];
    int numCategories;
    int parent;
    int size;
};

struct CategoryCount {
    char name[MAX_STRING_LEN];
    int count;
};

int compareCategoryCounts(const void *a, const void *b);

char **leafFiles(struct File *files, int numFiles, int *numLeafFiles);
char **kLargestCategories(struct File *files, int numFiles, int k, int *numReturned);
int largestFileSize(struct File *files, int numFiles);

int calculateTotalSize(struct File *files, int numFiles, int fileIndex);

int compareCategoryCounts(const void *a, const void *b) {
    const struct CategoryCount *countA = (const struct CategoryCount *)a;
    const struct CategoryCount *countB = (const struct CategoryCount *)b;

    if (countA->count > countB->count) {
        return -1;
    } else if (countA->count < countB->count) {
        return 1;
    }

    return strcmp(countA->name, countB->name);
}

char **leafFiles(struct File *files, int numFiles, int *numLeafFiles) {
    *numLeafFiles = 0;

    char **leafFileNames = malloc(numFiles * sizeof(char *));

    for (int i = 0; i < numFiles; i++) {
        bool isLeafFile = true;

        for (int j = 0; j < numFiles; j++) {
            if (files[j].parent == files[i].id) {
                isLeafFile = false;
                break;
            }
        }

        if (isLeafFile) {
            leafFileNames[*numLeafFiles] = strdup(files[i].name);
            (*numLeafFiles)++;
        }
    }

    return leafFileNames;
}

char **kLargestCategories(struct File *files, int numFiles, int k, int *numReturned) {
    struct CategoryCount categoryCounts[MAX_CATEGORIES];
    int numCategories = 0;

    for (int i = 0; i < numFiles; i++) {
        for (int j = 0; j < files[i].numCategories; j++) {
            char *category = files[i].categories[j];

            int categoryIndex;
            for (categoryIndex = 0; categoryIndex < numCategories; categoryIndex++) {
                if (strcmp(category, categoryCounts[categoryIndex].name) == 0) {
                    break;
                }
            }

            if (categoryIndex == numCategories) {
                strncpy(categoryCounts[numCategories].name, category, MAX_STRING_LEN);
                categoryCounts[numCategories].count = 0;
                numCategories++;
            }

            categoryCounts[categoryIndex].count++;
        }
    }

    qsort(categoryCounts, numCategories, sizeof(struct CategoryCount), compareCategoryCounts);

    char **resultCategories = malloc(k * sizeof(char *));
    *numReturned = k;

    for (int i = 0; i < k; i++) {
        resultCategories[i] = strdup(categoryCounts[i].name);
    }

    return resultCategories;
}

int calculateTotalSize(struct File *files, int numFiles, int fileIndex) {
    int totalSize = files[fileIndex].size;

    for (int j = 0; j < numFiles; j++) {
        if (files[j].parent == files[fileIndex].id) {
            totalSize += calculateTotalSize(files, numFiles, j);
        }
    }

    return totalSize;
}

int largestFileSize(struct File *files, int numFiles) {
    int largestSize = 0;

    for (int i = 0; i < numFiles; i++) {
        int totalSize = calculateTotalSize(files, numFiles, i);

        if (totalSize > largestSize) {
            largestSize = totalSize;
        }
    }

    return largestSize;
}

int qsortStrcmp(const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

int main(void) {
    struct File testFiles[] = {
        { .id = 1, .name = "Document.txt", .categories = {"Documents"}, .numCategories = 1, .parent = 3, .size = 1024 },
        { .id = 2, .name = "Image.jpg", .categories = {"Media", "Photos"}, .numCategories = 2, .parent = 34, .size = 2048 },
        { .id = 3, .name = "Folder", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 0 },
        { .id = 5, .name = "Spreadsheet.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 4096 },
        { .id = 8, .name = "Backup.zip", .categories = {"Backup"}, .numCategories = 1, .parent = 233, .size = 8192 },
        { .id = 13, .name = "Presentation.pptx", .categories = {"Documents", "Presentation"}, .numCategories = 2, .parent = 3, .size = 3072 },
        { .id = 21, .name = "Video.mp4", .categories = {"Media", "Videos"}, .numCategories = 2, .parent = 34, .size = 6144 },
        { .id = 34, .name = "Folder2", .categories = {"Folder"}, .numCategories = 1, .parent = 3, .size = 0 },
        { .id = 55, .name = "Code.py", .categories = {"Programming"}, .numCategories = 1, .parent = -1, .size = 1536 },
        { .id = 89, .name = "Audio.mp3", .categories = {"Media", "Audio"}, .numCategories = 2, .parent = 34, .size = 2560 },
        { .id = 144, .name = "Spreadsheet2.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 2048 },
        { .id = 233, .name = "Folder3", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 4096 },
    };

    int numLeafFiles;
    char *expectedLeafFiles[] = {
        "Audio.mp3",
        "Backup.zip",
        "Code.py",
        "Document.txt",
        "Image.jpg",
        "Presentation.pptx",
        "Spreadsheet.xlsx",
        "Spreadsheet2.xlsx",
        "Video.mp4"
    };
    char **returnedLeafFiles = leafFiles(testFiles, 12, &numLeafFiles);
    qsort(returnedLeafFiles, numLeafFiles, sizeof(char *), &qsortStrcmp);

    assert(numLeafFiles == 9);
    for (int i = 0; i < 9; i++) {
        assert(strcmp(returnedLeafFiles[i], expectedLeafFiles[i]) == 0);
        free(returnedLeafFiles[i]);
    }
    free(returnedLeafFiles);

    int numCategories;
    char *expectedCategories[] = {"Documents", "Folder", "Media"};
    char **returnedCategories = kLargestCategories(testFiles, 12, 3, &numCategories);
    assert(numCategories == 3);
        for (int i = 0; i < 3; i++) {
        assert(strcmp(returnedCategories[i], expectedCategories[i]) == 0);
        free(returnedCategories[i]);
    }
    free(returnedCategories);

    assert(largestFileSize(testFiles, 12) == 20992);

}