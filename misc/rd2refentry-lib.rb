require "English"

require "erb"
require "optparse"
require "rd/rdvisitor"
require "rd/version"

require "rt/rtparser"

require "gettext"
require "gettext/poparser"

module RD
  class RD2RefEntryVisitor < RDVisitor
    include ERB::Util

    SYSTEM_NAME = "RDtool -- RD2RefEntryVisitor"
    SYSTEM_VERSION = "0.0.1"
    VERSION = Version.new_from_version_string(SYSTEM_NAME, SYSTEM_VERSION)

    @@po = nil
    @@name = nil
    class << self
      def version
        VERSION
      end

      def po=(po)
        @@po = po
      end

      def name=(name)
        @@name = name
      end
    end

    # must-have constants
    OUTPUT_SUFFIX = "xml"
    INCLUDE_SUFFIX = ["xml"]

    METACHAR = {"<" => "&lt;", ">" => "&gt;", "&" => "&amp;"}

    def initialize
      super
      @translate_data = nil
      @n_sections = 0
      @n_text_blocks_in_section = 0
      @man_style = false
    end

    def apply_to_DocumentElement(element, contents)
      ret = ""
      ret << "#{xml_decl}\n"
      ret << "#{doctype_decl}\n"
      ret << "<refentry id=\"#{ref_entry_id}\">\n"
      ret << "#{ref_meta}\n"
      ret << "#{ref_name_div}\n"
      ret << collect_section_contents(contents).join("\n\n")
      ret << "</refentry>\n"
      ret
    end

    def apply_to_Headline(element, title)
      id = nil
      if /\A\s*\[(.+?)\]\s*/ =~ title.first
        id = $1
        title[0] = $POSTMATCH
      end
      case element.level
      when 1
        title = title.join
        @title, @purpose = title.split(/\s*\B---\B\s*/, 2)
        if @purpose.nil?
          components = title.split(/\s*\B\/\B\s*/)
          if components.size == 3
            name, package, manual = components
            @title = name
            @man_style = true
          end
        end
      when 2
        @n_sections += 1
        @n_text_blocks_in_section = 0
      end
      [:headline, element.level - 1, title, id]
    end

    def apply_to_Verbatim(element)
      lines = []
      element.each_line do |line|
        lines.push(line)
      end

      case lines.first.strip
      when "# RT"
        apply_to_RT(lines.join)
      else
        contents = lines.collect {|line| apply_to_String(line)}
        tag("programlisting", {}, contents.join("").chomp)
      end
    end

    def apply_to_RT(content)
      rt = RT::RTParser.parse(content)
      elements = [tag("caption", {}, apply_to_String(rt.config["caption"]))]

      header_rows = []
      rt.header.each do |row|
        header_cells = []
        row.each do |cell|
          if cell.class == RT::RTCell
            header_cells << [tag("th", {}, apply_to_String(cell.value))]
          end
        end
        header_rows << [tag("tr", {}, header_cells)]
      end
      elements << [tag("thead", {}, header_rows)]

      rows = []
      rt.body.each do |row|
        cells = []
        row.each do |cell|
          if cell.class == RT::RTCell
            cells << [tag("td", {}, apply_to_String(cell.value))]
          end
        end
        rows << [tag("tr", {}, cells)]
      end
      elements << [tag("tbody", {}, rows)]

      attributes = {}
      attributes["id"] = rt.config["id"] if rt.config["id"]
      tag("table", attributes, elements)
    end

    def apply_to_Reference_with_RDLabel(element, contents)
      if element.label.filename
        raise "label with filename is unsupported: #{element.label.inspect}"
      end
      url = element.label.element_label
      label = contents.join("").chomp
      label = url if label.empty?
      case url
      when /\.html$/
        if /\(\)$/ =~ label
          anchor = $PREMATCH.gsub(/_/, "-")
          url = "#{url}\##{anchor}"
        end
      when /\.html#[a-zA-Z\-_]+$/
      when /\.svg$/, /\.png$/
        url = url.sub(/svg$/, "png")
        return tag("inlinemediaobject",
                   {},
                   [
                    tag("imageobject", {},
                        tag("imagedata",
                            {
                              :fileref => url.sub(/svg$/, "png"),
                              :format => "PNG"
                            })),
                    tag("textobject", {}, tag("phrase", {}, label)),
                   ])
      else
        return tag("link",
                   {:linkend => ref_entry_id(url)},
                   remove_suffix(remove_lang_suffix(label)))
      end
      label = remove_suffix(remove_lang_suffix(label))
      tag("ulink", {:url => url}, label)
    end

    def apply_to_Reference_with_URL(element, contents)
      url = element.label.url
      label = contents.join("").chomp
      label = url if label.empty?
      tag("ulink", {:url => url}, label)
    end

    def apply_to_ItemList(element, items)
      tag("itemizedlist", {}, *items)
    end

    def apply_to_EnumList(element, items)
      tag("orderedlist", {}, *items)
    end

    def apply_to_DescList(element, items)
      tag("variablelist", {}, *items)
    end

    def apply_to_ItemListItem(element, contents)
      tag("listitem", {}, *contents)
    end

    def apply_to_EnumListItem(element, contents)
      tag("listitem", {}, *contents)
    end

    def apply_to_DescListItem(element, term, contents)
      tag("varlistentry", {},
          tag("term", {}, term),
          tag("listitem", {}, *contents))
    end

    def apply_to_TextBlock(element, contents)
      @n_text_blocks_in_section += 1
      if @man_style and @n_sections == 1 and @n_text_blocks_in_section == 1
        @purpose = contents.join.split(/\s+-\s+/, 2)[1]
      end
      tag("para", {}, *contents)
    end

    def apply_to_Emphasis(element, contents)
      tag("emphasis", {}, *contents)
    end

    def apply_to_Footnote(element, contents)
      tag("footnote", {},
          tag("para", {}, *contents))
    end

    def apply_to_Code(element, contents)
      tag("code", {}, *contents)
    end

    def apply_to_Var(element, contents)
      tag("varname", {}, *contents)
    end

    def apply_to_StringElement(element)
      apply_to_String(remove_newline(element.content))
    end

    def apply_to_String(element)
      h(element)
    end

    private
    def _(msgstr)
      translate_data[msgstr] || msgstr
    end

    def translate_data
      @translate_data ||= make_translate_data
    end

    def make_translate_data
      data = MOFile.new
      if @@po
        parser = GetText::PoParser.new
        parser.parse(File.read(@@po), data)
      end
      data
    end

    def xml_decl
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
    end

    def doctype_decl
      "<!DOCTYPE refentry \n" +
        "  PUBLIC \"-//OASIS//DTD DocBook XML V4.1.2//EN\"\n" +
        "  \"http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd\">"
    end

    def ref_entry_id(filename=nil)
      filename ||= ARGF.filename
      remove_suffix(File.basename(filename)).downcase
    end

    def tag(name, attributes={}, *contents)
      encoded_attributes = attributes.collect do |key, value|
        "#{key}=\"#{h(value)}\""
      end.join(" ")
      unless encoded_attributes.empty?
        encoded_attributes = " #{encoded_attributes}"
      end

      if contents.size == 1 and contents[0] !~ /</
        indented_contents = contents[0]
      else
        indented_contents = contents.collect do |content|
          "  #{content}"
        end.join("\n")
        indented_contents = "\n#{indented_contents}\n"
      end

      if indented_contents.empty?
        "<#{name}#{encoded_attributes}/>"
      else
        "<#{name}#{encoded_attributes}>#{indented_contents}</#{name}>"
      end
    end

    def ref_meta
      tag("refmeta", {},
          tag("refentrytitle",
              {
                "role" => "top_of_page",
                "id" => "#{ref_entry_id}.top_of_page",
              },
              @title),
          tag("refmiscinfo", {}, h(_("#{@@name || 'MY'} Library"))))
    end

    def ref_name_div
      contents = [tag("refname", {}, @title)]
      contents << tag("refpurpose", {}, @purpose) if @purpose
      tag("refnamediv", {}, *contents)
    end

    def collect_section_contents(contents)
      section_contents = pre_collect_section_contents(contents)
      post_collect_section_contents(section_contents)
    end

    def pre_collect_section_contents(contents)
      section_contents = [[0, [], nil]]
      contents.each do |content|
        if content.is_a?(Array) and content[0] == :headline
          _, level, title, id = content
          title_tag = tag("title", {}, *title)
          sub_section_contents = []
          while section_contents.last[0] > level
            sub_level, *sub_contents = section_contents.pop
            sub_section_contents.unshift(tag("refsect#{sub_level}", {},
                                             *sub_contents.flatten) + "\n")
          end
          section_contents.last[1].concat(sub_section_contents)
          section_contents << [level, [title_tag], id]
        else
          section_contents.last[1] << content
        end
      end
      section_contents
    end

    def post_collect_section_contents(section_contents)
      collected_section_contents = []
      section_contents.last[0].downto(0) do |level|
        sub_section_contents = []
        while section_contents.last[0] > level
          sub_level, sub_contents, id = section_contents.pop
          attributes = {}
          attributes["id"] = id if id
          sub_section_contents.unshift(tag("refsect#{sub_level}", attributes,
                                           *sub_contents))
        end
        unless sub_section_contents.empty?
          raise "!?" unless section_contents.last[0] == level
          _, contents, id = section_contents.pop
          if level > 0
            attributes = {}
            attributes["id"] = id if id
            contents = tag("refsect#{level}", attributes,
                           *(contents + sub_section_contents))
          else
            contents = sub_section_contents.join("\n\n")
          end
          collected_section_contents.unshift(contents + "\n")
        end
      end
      collected_section_contents
    end

    def consist_of_one_textblock?(listitem)
      listitem.children.size == 1 and listitem.children[0].is_a?(TextBlock)
    end

    def remove_lang_suffix(string)
      string.sub(/\.[a-z]{2}\z/, "")
    end

    def remove_suffix(string)
      string.sub(/(?:\.[^.]+)+\z/, "")
    end

    def remove_newline(string)
      content = ""
      lines = string.split(/\r?\n/)
      lines.each_with_index do |line, i|
        line = line.strip
        next_line = lines[i + 1]
        if next_line and /\A[a-z]/i =~ next_line and /[a-z]\z/i =~ line
          content << line + " "
        else
          content << line
        end
      end
      content
    end
  end
end

$Visitor_Class = RD::RD2RefEntryVisitor
ARGV.options do |opts|
  opts.on("--name=NAME", "The library name") do |name|
    RD::RD2RefEntryVisitor.name = name
  end

  opts.on("-pPO", "--po=PO", "PO file") do |po|
    RD::RD2RefEntryVisitor.po = po
  end
end
