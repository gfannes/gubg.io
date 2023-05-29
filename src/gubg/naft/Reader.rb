module Gubg
    module Naft

        class Reader
            def initialize(content)
                @enum = content.each_char().to_enum()
                @state = :block
                @stack = []
            end

            def next(&block)
                begin
                    ch = @enum.next()
                rescue StopIteration
                    case @state
                    when :node_text, :block_text
                        block.call(@state, @stage)
                    end
                    while !@stack.empty?()
                        node = @stack.pop()
                        block.call(:close_node, node)
                    end
                    raise()
                end

                # puts("#{@state} #{ch}")

                case @state
                when :block
                    case ch
                    when '['
                        @state = :reading_node
                        @stage = ''
                    when '}'
                        node = @stack.pop()
                        block.call(:close_node, node)
                    else
                        @state = :block_text
                        @stage = ch
                    end
                when :reading_node
                    case ch
                    when ']'
                        @state = :node
                        @stack.push(@stage)
                        block.(:open_node, @stage)
                        @stage = nil
                    else
                        @stage << ch
                    end
                when :node
                    case ch
                    when '('
                        @state = :reading_attr
                        @stage = ''
                    when '{'
                        @state = :block
                    when '['
                        node = @stack.pop()
                        block.call(:close_node, node)

                        @state = :reading_node
                        @stage = ''
                    else
                        @state = :node_text
                        @stage = ch
                    end
                when :reading_attr
                    case ch
                    when ')'
                        @state = :node
                        block.(:attr, @stage)
                        @stage = nil
                    else
                        @stage << ch
                    end
                when :node_text, :block_text
                    case ch
                    when '{'
                        block.call(@state, @stage)
                        @stage = nil
                        @state = :block
                    when '}'
                        block.call(@state, @stage)
                        @stage = nil
                        @state = :block

                        node = @stack.pop()
                        block.call(:close_node, node)
                    when '['
                        block.call(@state, @stage)
                        @stage = ''
                        @state = :reading_node
                    else
                        @stage << ch
                    end
                end
            end

            def each(&block)
                loop do
                    begin
                        self.next(&block)
                    rescue StopIteration
                    break
                    end
                end
            end
        end

    end
end
