require('gubg/naft/Reader')
require('gubg/catch')
include Gubg::Catch

test_case("Reader") do
    content = nil

    section('Single node') do
        content = '[Node]'
    end
    section('Node with attrs') do
        content = '[Node](attr0)(attr1)'
    end
    section('Two nodes') do
        content = '[Node0][Node1]'
    end
    section('Free text') do
        content = '[Node]abc'
    end
    section('Free text') do
        content = '[Node]{abc}'
    end

    reader = Gubg::Naft::Reader.new(content)
    reader.each do |event, str|
        puts("#{event}: #{str}")
    end
end

Gubg::Catch.run()