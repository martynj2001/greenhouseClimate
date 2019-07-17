require "application_system_test_case"

class MeasuresTest < ApplicationSystemTestCase
  setup do
    @measure = measures(:one)
  end

  test "visiting the index" do
    visit measures_url
    assert_selector "h1", text: "Measures"
  end

  test "creating a Measure" do
    visit measures_url
    click_on "New Measure"

    fill_in "Temp in", with: @measure.temp_in
    fill_in "Temp out", with: @measure.temp_out
    click_on "Create Measure"

    assert_text "Measure was successfully created"
    click_on "Back"
  end

  test "updating a Measure" do
    visit measures_url
    click_on "Edit", match: :first

    fill_in "Temp in", with: @measure.temp_in
    fill_in "Temp out", with: @measure.temp_out
    click_on "Update Measure"

    assert_text "Measure was successfully updated"
    click_on "Back"
  end

  test "destroying a Measure" do
    visit measures_url
    page.accept_confirm do
      click_on "Destroy", match: :first
    end

    assert_text "Measure was successfully destroyed"
  end
end
