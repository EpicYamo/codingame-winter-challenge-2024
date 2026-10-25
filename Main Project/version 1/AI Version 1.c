#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct id_list_all
{
    int myorganid; // id of this entity if it's an organ, 0 otherwise
    int organrootid; // root id of the organ
    int ownerid; // 1 if your organ, 0 if enemy organ, -1 if neither
    int organparentid; // previous organ id of the current organ id
    int xx; // grid coordinate
    int yy; // grid coordinate
    char *ttype; // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D
    char *organdirr; // N,E,S,W or X if not an organ
} id_list;

typedef struct coordinates_s
{
    int x1;
    int y1;
} coordinates;

typedef struct layer_one_s
{
    coordinates up;
    coordinates down;
    coordinates right;
    coordinates left;
} layer_one;

typedef struct layer_two_s
{
    coordinates up_up;
    coordinates right_up;
    coordinates left_up;
    coordinates down_up;

    coordinates up_down;
    coordinates right_down;
    coordinates left_down;
    coordinates down_down;
    
    coordinates up_right;
    coordinates right_right;
    coordinates left_right;
    coordinates down_right;

    coordinates up_left;
    coordinates right_left;
    coordinates left_left;
    coordinates down_left;
} layer_two;

typedef struct move_id_list_s
{
    int validity; // 1 if its a possible move / 0 if its not
    int myorganidm; // id of this entity if it's an organ, 0 otherwise
    int organrootidm; // root id of the organ
    int owneridm; // 1 if your organ, 0 if enemy organ, -1 if neither
    int organparentidm; // previous organ id of the current organ id
    int xxm; // grid coordinate
    int yym; // grid coordinate
    char *ttypem; // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D
    char *organdirrm; // N,E,S,W or X if not an organ
    char *move_loc; //   U   ,   D   ,   R   ,  L    ,  N
                    //   UP    DOWN    RIGHT   LEFT    NONE
} move_id_list;

int             root_counter(id_list *my_list, int entity_count);
int             protein_counter(id_list *my_list, int entity_count);
id_list         *last_organ_finder_per_root(id_list *my_list, int entity_count);
id_list         *protein_information_logger(id_list *my_list, int entity_count);
id_list         *store_roots_func(id_list *my_list, int entity_count);
void            ft_engine(id_list organ, id_list *my_list, int entity_count, int width, int height);
layer_one       set_layer_one(id_list organ);
layer_two       set_layer_two(layer_one layerone);
move_id_list    *check_validity_layerone(layer_one layerone, id_list *my_list, int entity_count, int width, int height);
char            find_from_coordinates(id_list *my_list, int count_tmp, int x, int y);

void ft_engine(id_list organ, id_list *my_list, int entity_count, int width, int height)
{
    int free_counter = 0;
    int i = 0;
    int moved = 0;
    layer_one layerone;
    layer_one backup;
    move_id_list *possible_moves;
    move_id_list *possible_moves_backup;
    id_list *roots;

    layerone = set_layer_one(organ);
    possible_moves = check_validity_layerone(layerone, my_list, entity_count, width, height);
    while (i < 4 && moved == 0)
    {
        if (possible_moves[i].validity == 1)
        {
            moved++;
            printf("GROW %d %d %d BASIC N\n", organ.myorganid, possible_moves[i].xxm, possible_moves[i].yym);
        }
        i++;
    }
    i = 0;
    if (moved == 0)
    {
        i = 0;
        roots = store_roots_func(my_list, entity_count);
        backup = set_layer_one(roots[0]);
        possible_moves_backup = check_validity_layerone(backup, my_list, entity_count, width, height);
        fprintf(stderr, "%d", i);
        while (i < 4 && moved == 0)
        {
            if (possible_moves_backup[i].validity == 1)
            {
                moved++;
                printf("GROW %d %d %d BASIC N\n", roots[0].myorganid, possible_moves_backup[i].xxm, possible_moves_backup[i].yym);
            }
            i++;
        }
        free(possible_moves_backup);
        free(roots);
    }

    while (free_counter < 4)
    {
        free(possible_moves[free_counter].move_loc);
        free_counter++;
    }
    free(possible_moves);
}

int main()
{
    int i; // important
    int width;     // columns in the game grid
    int height;     // rows in the game grid
    scanf("%d%d", &width, &height);

    int move_count = 0;
    while (1) 
    {
        int entity_count;
        scanf("%d", &entity_count);
        
        id_list my_list[entity_count];

        id_list *last_organ_per_root;
        id_list *protein_info;

        int root_count;
        int j = 0;

        i = 0; 
        while (i < entity_count) 
        {
            int x;
            int y;
            char type[162][33];
            int owner;
            int organ_id;
            char organ_dir[162][2];
            int organ_parent_id;
            int organ_root_id;
            scanf("%d%d%s%d%d%s%d%d", &x, &y, type[i], &owner, &organ_id, organ_dir[i], &organ_parent_id, &organ_root_id);
            my_list[i].ownerid = owner;
            my_list[i].myorganid = organ_id;
            my_list[i].organparentid = organ_parent_id;
            my_list[i].organrootid = organ_root_id;
            my_list[i].organdirr = organ_dir[i];
            my_list[i].xx = x;
            my_list[i].yy = y;
            my_list[i].ttype = type[i];
            i++;
        }
        protein_info = protein_information_logger(my_list, entity_count);
        last_organ_per_root = last_organ_finder_per_root(my_list, entity_count);
        root_count = root_counter(my_list, entity_count);

        int my_a;
        int my_b;
        int my_c;
        int my_d;
        scanf("%d%d%d%d", &my_a, &my_b, &my_c, &my_d);
        int opp_a;
        int opp_b;
        int opp_c;
        int opp_d;
        scanf("%d%d%d%d", &opp_a, &opp_b, &opp_c, &opp_d);

        int required_actions_count;
        scanf("%d", &required_actions_count);

        while (j < required_actions_count && root_count > 0) 
        {
            ft_engine(last_organ_per_root[root_count - 1], my_list, entity_count, width, height);
            root_count--;
            j++;
        }
        move_count++;
        free(last_organ_per_root);
        free(protein_info);
    }
    return (0);
}

id_list *store_roots_func(id_list *my_list, int entity_count)
{
    id_list *root_infos = calloc(100, sizeof(id_list));

    int root_count;
    root_count = root_counter(my_list, entity_count);

    int root_flag = 0;
    
    entity_count -= 1;
    while (entity_count >= 0 && root_count > 0)
    {
        while (root_flag == 0)
        {
            if ((my_list[entity_count].ownerid == 1) && (my_list[entity_count].ttype[0] == 'R'))
            {
                root_infos[root_count - 1] = my_list[entity_count];
                root_flag += 1;
            }
            entity_count--;
        }
        root_flag = 0;
        root_count -= 1;
    }
    return (root_infos);
}

int protein_counter(id_list *my_list, int entity_count)
{
    int protein_count = 0;
    
    entity_count -= 1;
    while (entity_count >= 0)
    {
        if (my_list[entity_count].ttype[0] == 'A' || my_list[entity_count].ttype[0] == 'B' || my_list[entity_count].ttype[0] == 'C'|| my_list[entity_count].ttype[0] == 'D')
            protein_count++;
        entity_count--;
    }
    return (protein_count);
}

int root_counter(id_list *my_list, int entity_count)
{
    int root_count = 0;

    entity_count -= 1;
    while (entity_count >= 0)
    {
        if ((my_list[entity_count].ownerid == 1) && (my_list[entity_count].ttype[0] == 'R'))
            root_count += 1;
        entity_count--;
    }
    return (root_count);
}

id_list *last_organ_finder_per_root(id_list *my_list, int entity_count)
{
    id_list *last_organ_per_root = calloc(100, sizeof(id_list));

    int root_count;
    root_count = root_counter(my_list, entity_count);

    id_list *root_infos;
    root_infos = store_roots_func(my_list, entity_count);

    int entc_tmp;
    int i = 0;

    int organ_id_flag;

    while (root_count > 0)
    {
        entc_tmp = entity_count - 1;
        organ_id_flag = 0;

        while (entc_tmp >= 0)
        {
            if ((my_list[entc_tmp].ownerid) == 1 && (organ_id_flag < my_list[entc_tmp].myorganid) && (my_list[entc_tmp].organrootid == root_infos[i].organrootid))
            {
                organ_id_flag = my_list[entc_tmp].myorganid;
                last_organ_per_root[i] = my_list[entc_tmp];
            }
            entc_tmp--;
        }
        i++;
        root_count--;
    }
    free (root_infos);
    return (last_organ_per_root);
}

id_list *protein_information_logger(id_list *my_list, int entity_count)
{
    int i = 0;
    entity_count -= 1;
    id_list *protein_info = calloc(100, sizeof(id_list));

    while (entity_count >= 0)
    {
        if (my_list[entity_count].ttype[0] == 'A' || my_list[entity_count].ttype[0] == 'B' || my_list[entity_count].ttype[0] == 'C'|| my_list[entity_count].ttype[0] == 'D')
        {
            protein_info[i] = my_list[entity_count];
            i++;
        }
        entity_count--;
    }
    return (protein_info);
}

layer_one set_layer_one(id_list organ)
{
    layer_one layerone;

    layerone.up.x1 = organ.xx;
    layerone.up.y1 = organ.yy - 1;
    layerone.down.x1 = organ.xx;
    layerone.down.y1 = organ.yy + 1;
    layerone.right.x1 = organ.xx + 1;
    layerone.right.y1 = organ.yy;
    layerone.left.x1 = organ.xx - 1;
    layerone.left.y1 = organ.yy;

    return (layerone);
}

layer_two set_layer_two(layer_one layerone)
{
    layer_two layertwo;

    layertwo.up_up.x1 = layerone.up.x1;
    layertwo.up_up.y1 = layerone.up.y1 - 1;
    layertwo.right_up.x1 = layerone.up.x1 + 1;
    layertwo.right_up.y1 = layerone.up.y1;
    layertwo.left_up.x1 = layerone.up.x1 - 1;
    layertwo.left_up.y1 = layerone.up.y1;
    layertwo.down_up.x1 = layerone.up.x1;
    layertwo.down_up.y1 = layerone.up.y1 + 1;
    layertwo.up_down.x1 = layerone.down.x1;
    layertwo.up_down.y1 = layerone.down.y1 - 1;
    layertwo.right_down.x1 = layerone.down.x1 + 1;
    layertwo.right_down.y1 = layerone.down.y1;
    layertwo.left_down.x1 = layerone.down.x1 - 1;
    layertwo.left_down.y1 = layerone.down.y1;
    layertwo.down_down.x1 = layerone.down.x1;
    layertwo.down_down.y1 = layerone.down.y1 + 1;
    layertwo.up_right.x1 = layerone.right.x1;
    layertwo.up_right.y1 = layerone.right.y1 - 1;
    layertwo.right_right.x1 = layerone.right.x1 + 1;
    layertwo.right_right.y1 = layerone.right.y1;
    layertwo.left_right.x1 = layerone.right.x1 - 1;
    layertwo.left_right.y1 = layerone.right.y1;
    layertwo.down_right.x1 = layerone.right.x1;
    layertwo.down_right.y1 = layerone.right.y1 + 1;
    layertwo.up_left.x1 = layerone.left.x1;
    layertwo.up_left.y1 = layerone.left.y1 - 1;
    layertwo.right_left.x1 = layerone.left.x1 + 1;
    layertwo.right_left.y1 = layerone.left.y1;
    layertwo.left_left.x1 = layerone.left.x1 - 1;
    layertwo.left_left.y1 = layerone.left.y1;
    layertwo.down_left.x1 = layerone.left.x1;
    layertwo.down_left.y1 = layerone.left.y1 + 1;

    return (layertwo);
}

move_id_list    *check_validity_layerone(layer_one layerone, id_list *my_list, int entity_count, int width, int height)
{
    move_id_list    *possible_moves = calloc(4, sizeof(move_id_list));
    int             count_tmp;

    char up[2];
    up[0] = '\0';
    up[1] = '\0';
    char down[2];
    down[0] = '\0';
    down[1] = '\0';
    char right[2];
    right[0] = '\0';
    right[1] = '\0';
    char left[2];
    left[0] = '\0';
    left[1] = '\0';

    count_tmp = entity_count - 1;

    up[0] = find_from_coordinates(my_list, count_tmp, layerone.up.x1, layerone.up.y1);
    down[0] = find_from_coordinates(my_list, count_tmp, layerone.down.x1, layerone.down.y1);
    right[0] = find_from_coordinates(my_list, count_tmp, layerone.right.x1, layerone.right.y1);
    left[0] = find_from_coordinates(my_list, count_tmp, layerone.left.x1, layerone.left.y1);

    possible_moves[0].validity = 0;
    possible_moves[1].validity = 0;
    possible_moves[2].validity = 0;
    possible_moves[3].validity = 0;

    
    if (up[0] == 'N' || up[0] == 'A' || up[0] == 'B' || up[0] == 'C' || up[0] == 'D')
    {
        if (!(layerone.up.x1 < 0 || layerone.up.y1 < 0 || layerone.up.x1 > width || layerone.up.y1 > height))
        {
            possible_moves[0].move_loc =  calloc(2, sizeof(char));
            possible_moves[0].move_loc[0] = 'U';
            possible_moves[0].move_loc[1] = '\0';
            possible_moves[0].validity = 1;
            possible_moves[0].xxm = layerone.up.x1;
            possible_moves[0].yym = layerone.up.y1;
            possible_moves[0].ttypem = up[0];
        }
    }

    if (possible_moves[0].validity == 0)
    {
        possible_moves[0].move_loc =  calloc(2, sizeof(char));
        possible_moves[0].move_loc[0] = 'N';
        possible_moves[0].move_loc[1] = '\0';
    }

    if (down[0] == 'N' || down[0] == 'A' || down[0] == 'B' || down[0] == 'C' || down[0] == 'D')
    {
        if (!(layerone.down.x1 < 0 || layerone.down.y1 < 0 || layerone.down.x1 > width || layerone.down.y1 > height))
        {
            possible_moves[1].move_loc =  calloc(2, sizeof(char));
            possible_moves[1].move_loc[0] = 'D';
            possible_moves[1].move_loc[1] = '\0';
            possible_moves[1].validity = 1;
            possible_moves[1].xxm = layerone.down.x1;
            possible_moves[1].yym = layerone.down.y1;
            possible_moves[1].ttypem = down[0];
        }
    }

    if (possible_moves[1].validity == 0)
    {
        possible_moves[1].move_loc =  calloc(2, sizeof(char));
        possible_moves[1].move_loc[0] = 'N';
        possible_moves[1].move_loc[1] = '\0';
    }

    if (right[0] == 'N' || right[0] == 'A' || right[0] == 'B' || right[0] == 'C' || right[0] == 'D')
    {
        if (!(layerone.right.x1 < 0 || layerone.right.y1 < 0 || layerone.right.x1 > width || layerone.right.y1 > height))
        {
            possible_moves[2].move_loc =  calloc(2, sizeof(char));
            possible_moves[2].move_loc[0] = 'R';
            possible_moves[2].move_loc[1] = '\0';
            possible_moves[2].validity = 1;
            possible_moves[2].xxm = layerone.right.x1;
            possible_moves[2].yym = layerone.right.y1;
            possible_moves[2].ttypem = right[0];
        }
    }

    if (possible_moves[2].validity == 0)
    {
        possible_moves[2].move_loc =  calloc(2, sizeof(char));
        possible_moves[2].move_loc[0] = 'N';
        possible_moves[2].move_loc[1] = '\0';
    }

    if (left[0] == 'N' || left[0] == 'A' || left[0] == 'B' || left[0] == 'C' || left[0] == 'D')
    {
        if (!(layerone.left.x1 < 0 || layerone.left.y1 < 0 || layerone.left.x1 > width || layerone.left.y1 > height))
        {
            possible_moves[3].move_loc =  calloc(2, sizeof(char));
            possible_moves[3].move_loc[0] = 'L';
            possible_moves[3].move_loc[1] = '\0';
            possible_moves[3].validity = 1;
            possible_moves[3].xxm = layerone.left.x1;
            possible_moves[3].yym = layerone.left.x1;
            possible_moves[3].ttypem = left[0];
        }
    }

    if (possible_moves[3].validity == 0)
    {
        possible_moves[3].move_loc =  calloc(2, sizeof(char));
        possible_moves[3].move_loc[0] = 'N';
        possible_moves[3].move_loc[1] = '\0';
    }

    return (possible_moves);
}

char    find_from_coordinates(id_list *my_list, int count_tmp, int x, int y)
{
    while (count_tmp >= 0)
    {
        if (my_list[count_tmp].xx == x && my_list[count_tmp].yy == y)
        {
            if (my_list[count_tmp].ttype[0] == 'W' || my_list[count_tmp].ttype[0] == 'R' || my_list[count_tmp].ttype[0] == 'B' || my_list[count_tmp].ttype[0] == 'T' || my_list[count_tmp].ttype[0] == 'H' || my_list[count_tmp].ttype[0] == 'S' || my_list[count_tmp].ttype[0] == 'A' || my_list[count_tmp].ttype[0] == 'B' || my_list[count_tmp].ttype[0] == 'C' || my_list[count_tmp].ttype[0] == 'D')
            {
                if (my_list[count_tmp].ttype[0] == 'B' && my_list[count_tmp].ttype[1] == 'A')
                    return ('G');
                else
                    return (my_list[count_tmp].ttype[0]);
            }        
        }
        count_tmp--;
    }
    return ('N');
}