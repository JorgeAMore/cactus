#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>

#include "cactus.h"
#include "normal.h"

/*
 * This script is run bottom up on a cactus tree and ensures the tree is 'normalised' as we expect.
 */

void usage() {
    fprintf(stderr, "cactus_normalisation [flower names], version 0.1\n");
    fprintf(stderr, "-a --logLevel : Set the log level\n");
    fprintf(stderr,
            "-c --cactusDisk : The location of the flower disk directory\n");
    fprintf(
            stderr,
            "-d --maxNumberOfChains : The maximum number of individual chains to promote into a flower.\n");
    fprintf(stderr, "-h --help : Print this help screen\n");
}

int main(int argc, char *argv[]) {
    /*
     * Script for adding a reference genome to a flower.
     */

    /*
     * Arguments/options
     */
    char * logLevelString = NULL;
    char * cactusDiskName = NULL;
    int32_t j;
    int32_t maxNumberOfChains = 0;

    ///////////////////////////////////////////////////////////////////////////
    // (0) Parse the inputs handed by genomeCactus.py / setup stuff.
    ///////////////////////////////////////////////////////////////////////////

    while (1) {
        static struct option long_options[] = { { "logLevel",
                required_argument, 0, 'a' }, { "cactusDisk", required_argument,
                0, 'c' }, { "maxNumberOfChains", required_argument, 0, 'd' },
                { "help", no_argument, 0, 'h' }, { 0, 0, 0, 0 } };

        int option_index = 0;

        int key = getopt_long(argc, argv, "a:c:d:h", long_options, &option_index);

        if (key == -1) {
            break;
        }

        switch (key) {
            case 'a':
                logLevelString = stString_copy(optarg);
                break;
            case 'c':
                cactusDiskName = stString_copy(optarg);
                break;
            case 'd':
                j = sscanf(optarg, "%i", &maxNumberOfChains);
                assert(j == 1);
                break;
            case 'h':
                usage();
                return 0;
            default:
                usage();
                return 1;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // (0) Check the inputs.
    ///////////////////////////////////////////////////////////////////////////

    assert(logLevelString == NULL || strcmp(logLevelString, "CRITICAL") == 0 || strcmp(logLevelString, "INFO") == 0 || strcmp(logLevelString, "DEBUG") == 0);
    assert(cactusDiskName != NULL);

    //////////////////////////////////////////////
    //Set up logging
    //////////////////////////////////////////////

    if (logLevelString != NULL && strcmp(logLevelString, "INFO") == 0) {
        st_setLogLevel(ST_LOGGING_INFO);
    }
    if (logLevelString != NULL && strcmp(logLevelString, "DEBUG") == 0) {
        st_setLogLevel(ST_LOGGING_DEBUG);
    }

    //////////////////////////////////////////////
    //Log (some of) the inputs
    //////////////////////////////////////////////

    st_logInfo("Flowerdisk name : %s\n", cactusDiskName);

    //////////////////////////////////////////////
    //Load the database
    //////////////////////////////////////////////

    CactusDisk *cactusDisk = cactusDisk_construct(cactusDiskName);
    st_logInfo("Set up the flower disk\n");

    ///////////////////////////////////////////////////////////////////////////
    // Loop on the flowers, doing the reference genome (this process must be run bottom up)
    ///////////////////////////////////////////////////////////////////////////

    for (j = optind; j < argc; j++) {
        /*
         * Read the flower.
         */
        const char *flowerName = argv[j];
        st_logInfo("Processing the flower named: %s\n", flowerName);
        Flower *flower = cactusDisk_getFlower(cactusDisk,
                cactusMisc_stringToName(flowerName));
        assert(flower != NULL);
        st_logInfo("Parsed the flower to normalise\n");

        /*
         * Now run the normalisation functions
         */
#ifdef BEN_DEBUG
        flower_check(flower);
#endif
        promoteChainsThatExtendHigherLevelChains(flower);
        promoteChainsToFillParents(flower, maxNumberOfChains);
        removeTrivialLinks(flower);
#ifdef BEN_DEBUG
        flower_check(flower);
#endif
        if (!flower_deleteIfEmpty(flower)) { //If we delete the flower we need not run the remaining functions..
            flower_makeTerminalNormal(flower);
            flower_removeIfRedundant(flower); //This may destroy the flower, but nots its children..
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Write the flower(s) back to disk.
    ///////////////////////////////////////////////////////////////////////////

    cactusDisk_write(cactusDisk);
    st_logInfo("Updated the flower on disk\n");

    ///////////////////////////////////////////////////////////////////////////
    //Clean up.
    ///////////////////////////////////////////////////////////////////////////

    //Destruct stuff
    cactusDisk_destruct(cactusDisk);

    st_logInfo("Cleaned stuff up and am finished\n");
    return 0;
}
