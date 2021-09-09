# RTS_Resource_Manager
Portfolio Project for SMU Guildhall Application

## Intent

User friendly developement tool that manages a resource tree for a hypothetical RTS game.

## Scope

Node manager program that supports creating and deleting nodes, as well as creating and deleting connections between those nodes.
ImGui based user interface.
Nodes can have any number of dependencies.
Should display a visual graph of the currently selected node branching into it's dependencies all the way down to the end of each branch.

## Input

resource.txt file which contains resources and the resources they depend on. Example format:  
handgun bullets  
bullets ore  
bombs ore  
turret bullets  

## Output

Updated resource.txt
