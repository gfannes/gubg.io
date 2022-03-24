namespace(:gubg) do |ns|
    ns.tuple(:nil)

    ns.tuple(:i3) do |t|
        t.int(:x)
        t.int(:y)
        t.int(:z)
    end
    ns.tuple(:f3) do |t|
        t.float(:x)
        t.float(:y)
        t.float(:z)
    end
end