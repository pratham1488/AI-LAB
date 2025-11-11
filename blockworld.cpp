% --------------------------------------------------
%  BLOCK WORLD — Breadth-First Search (fixed)
% --------------------------------------------------

% A state is: state(OnList, ClearList, Holding)
% OnList  = [on(a,table), on(b,a), ...]
% ClearList = [clear(x), clear(y), ...]
% Holding = name of block held or 'none'

% --- Library helpers ---
% Using built-in predicates: member/2, delete/3, reverse/2

% -------------------- INITIAL / GOAL --------------------

initial(
    state(
        [on(a,table), on(b,table), on(c,a)],
        [clear(b), clear(c), clear(table)],
        none
    )
).

goal(
    state(
        [on(b,c), on(c,a), on(a,table)],
        [clear(b), clear(table)],
        none
    )
).

% -------------------- STATE MATCHING --------------------

% Check if two states are equivalent (order-independent comparison)
states_equal(state(On1, Clear1, H), state(On2, Clear2, H)) :-
    same_elements(On1, On2),
    same_elements(Clear1, Clear2).

% Check if two lists contain the same elements (order independent)
same_elements([], []).
same_elements([H|T], L2) :-
    member(H, L2),
    delete(L2, H, L2Rest),
    same_elements(T, L2Rest).

% -------------------- OPERATORS --------------------

% 1. Unstack X from Y (Y ≠ table)
move(
    state(On, Clear, none),
    unstack(X, Y),
    state(NewOn, [clear(Y)|TempClear], X)
) :-
    member(on(X,Y), On),
    Y \= table,
    member(clear(X), Clear),
    delete(On, on(X,Y), NewOn),
    delete(Clear, clear(X), TempClear).

% 2. Put block X down on table
move(
    state(On, Clear, X),
    putdown(X),
    state([on(X,table)|On], [clear(X)|Clear], none)
) :-
    X \= none.

% 3. Pick up block X from table
move(
    state(On, Clear, none),
    pickup(X),
    state(NewOn, NewClear, X)
) :-
    member(on(X,table), On),
    member(clear(X), Clear),
    delete(On, on(X,table), NewOn),
    delete(Clear, clear(X), NewClear).

% 4. Stack X on Y
move(
    state(On, Clear, X),
    stack(X, Y),
    state([on(X,Y)|On], [clear(X)|TempClear], none)
) :-
    X \= none,
    member(clear(Y), Clear),
    delete(Clear, clear(Y), TempClear).

% -------------------- BREADTH-FIRST SEARCH --------------------

% Base Case: If the head state matches the Goal, we found the solution.
bfs([[State|Path]|_], Goal, [State|Path]) :-
    states_equal(State, Goal), !.

% Recursive Step:
bfs([[State|Path]|Rest], Goal, Solution) :-
    % Find all valid next states
    findall([Next,State|Path],
        (move(State,_,Next), \+ state_in_path(Next,[State|Path])),
        NewPaths),
    % Append new paths to the end of the queue (BFS FIFO)
    append(Rest, NewPaths, Queue),
    bfs(Queue, Goal, Solution).

% Check if a state already exists in the path (to avoid cycles)
state_in_path(State, [H|_]) :-
    states_equal(State, H), !.
state_in_path(State, [_|T]) :-
    state_in_path(State, T).

% -------------------- SOLVER --------------------

solve :-
    initial(I),
    goal(G),
    nl,
    write('============================================================'), nl,
    write('           BLOCK WORLD PROBLEM SOLVER (BFS)'), nl,
    write('============================================================'), nl, nl,
    
    write('INITIAL STATE:'), nl,
    print_state(I), nl,
    
    write('GOAL STATE:'), nl,
    print_state(G), nl,
    
    write('Searching for solution...'), nl, nl,
    
    % Start BFS with the initial state in a path list: [[InitialState]]
    bfs([[I]], G, RevPath),
    
    % Reverse the resulting path (which is currently [Goal, ..., Initial])
    reverse(RevPath, Path),
    length(Path, Len),
    Steps is Len - 1,
    
    write('============================================================'), nl,
    write('                   SOLUTION FOUND'), nl,
    write('============================================================'), nl,
    format('Total steps: ~w~n~n', [Steps]),
    
    print_plan(Path, 1),
    
    write('============================================================'), nl,
    write('                   GOAL ACHIEVED'), nl,
    write('============================================================'), nl, nl.

% -------------------- PLAN PRINTER --------------------

% Pretty print a state
print_state(state(On, Clear, Holding)) :-
    write('  On relations: '),
    print_list(On), nl,
    write('  Clear blocks: '),
    print_list(Clear), nl,
    write('  Holding: '), write(Holding), nl.

% Print a list nicely
print_list([]) :- write('[]').
print_list([H]) :- write(H), !.
print_list([H|T]) :-
    write(H), write(', '),
    print_list(T).

% Base case: finished printing (single state, no more actions)
print_plan([_], _).

% Recursive step: find the action that connects S1 to S2 and print it.
print_plan([S1,S2|Rest], StepNum) :-
    move(S1, Action, S2), !,
    write('------------------------------------------------------------'), nl,
    format('STEP ~w: ~w~n', [StepNum, Action]),
    write('------------------------------------------------------------'), nl,
    print_state(S2), nl,
    NextStep is StepNum + 1,
    print_plan([S2|Rest], NextStep).

% -------------------- UTILITY --------------------
% Using built-in reverse/2
