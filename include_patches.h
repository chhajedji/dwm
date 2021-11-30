
/* This file includes macros for some of the patches. To disable a
 * patch, just set value to 0. */

#define CENTERED_MASTER         (0)
#define HIDE_VACANT_TAGS        (0)
#define ATTACH_ASIDE            (1)
#define STATUSCOLOR             (0)
#define FULL_SCREEN             (0)
#define STATUSCMD               (1)
#define PYWAL                   (0)
#define SCRATCHPADS             (1)

/*
 * WINDOWINDICATION and ACTIVEBARINDICATION is adviced to be disabled if
 * HIDE_VACANT_TAGS is enabled. Why indicate something which is already
 * visible?
 */
#define WINDOWINDICATION        (1) /* Not any official patch. My changes to hide indication on tags having some window. */
#define ACTIVEBARINDICATION     (1) /* If both WINDOWINDICATION and ACTIVEBARINDICATION are set, latter takes priority over former. */
