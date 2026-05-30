open System
open System.IO

type AllocPlan =
    { Name : string
      Size : int }

let plan =
    [
        { Name = "buffer"; Size = 1024 }
        { Name = "big_buffer"; Size = 4096 }
    ]

let lines =
    plan
    |> List.map (fun p -> p.Name + " " + string p.Size)

File.WriteAllLines("alloc_plan.txt", lines)

printfn "Control plane: wrote alloc_plan.txt"
