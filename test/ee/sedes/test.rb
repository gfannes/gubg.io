mod(:gubg) do
    typedef(:myint, :int)

    tuple(:nil)

    tuple(:i3) do
        single(:x, :int)
        single(:y, :int)
        single(:z, :int)
    end
    tuple(:f3) do
        single(:x, :float)
        single(:y, :float)
        single(:z, :float)
    end
    tuple(:a3) do
        array(:values, :float, 3)
    end
end